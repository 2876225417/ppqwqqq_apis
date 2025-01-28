
#include "article_repository.h"

#include <iostream>

article_repository::article_repository(postgres_conn_pool& pool)
    : m_connection_pool(pool) { 
    
    auto conn = m_connection_pool.acquire();
    pqxx::work tx(conn->get_raw_connection());
    tx.conn().prepare(
        "create_article",
        "INSERT INTO articles (title, content, author, created_at, published) "
        "VALUES ($1, $2, $3, NOW(), $4) RETURNING id"
    );
    tx.commit();
    m_connection_pool.release(conn);
}

std::vector<article> article_repository::get_all_published_aritcles() {
    auto conn = m_connection_pool.acquire();
    std::vector<article> articles;
    try {
        pqxx::work tx(conn->get_raw_connection());
        auto result = tx.exec(
            "SELECT id, title, content, author, created_at, updated_at "
            "FROM articles WHERE published = true ORDER BY created_at DESC"
        );
        for (const auto& row: result) {
            articles.push_back(map_row_to_article(row));
        }

        tx.commit();
    } catch (...) {
        m_connection_pool.release(conn);
        throw;
    }

    m_connection_pool.release(conn);
    return articles;
}

std::optional<article> article_repository::get_article_by_id(int id) {
    auto conn = m_connection_pool.acquire();

    try {
        pqxx::work tx(conn->get_raw_connection());
        tx.conn().prepare(
            "get_by_id",
            "SELECT * FROM articles WHERE id = $1"
        );
        auto result = tx.exec_prepared("get_by_id", id);
        if (!result.empty()) 
            return map_row_to_article(result[0]);

        tx.commit();
    } catch (...) {
        m_connection_pool.release(conn);
        throw;
    }
    return std::nullopt;
}

int article_repository::create_article(const article& article) {
    auto conn = m_connection_pool.acquire();
    
    try {
        pqxx::work tx(conn->get_raw_connection());
        auto result = tx.exec_prepared("create_article",
            article.title,
            article.content,
            article.author,
            article.published
        );
        tx.commit();
        return result[0][0].as<int>();
    } catch (...) {
        m_connection_pool.release(conn);
        throw;
    }
}

article article_repository::map_row_to_article(const pqxx::row& row) {
    return article {
        .id = row["id"].as<int>(),
        .title = row["title"].as<std::string>(),
        .content = row["content"].as<std::string>(),
        .author = row["author"].as<std::string>(),
        .created_at = article::from_pg_timestamp(row["created_at"].as<std::string>()),
        .updated_at = article::from_pg_timestamp(row["updated_at"].as<std::string>())
    };
}

std::chrono::system_clock::time_point
article::from_pg_timestamp(const std::string& pg_time) {
    std::tm tm = {};
    std::istringstream ss(pg_time);    
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string article::to_pg_timestamp(const std::chrono::system_clock::time_point& tp) {
    auto in_time_t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm;
    localtime_r(&in_time_t, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}