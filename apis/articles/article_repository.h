

#ifndef ARTICLE_REPOSITORY_H
#define ARTICLE_REPOSITORY_H

#include "../db_utils/conn_pool.h"
#include "article.h"
#include <optional>
#include <pqxx/pqxx>
#include <vector>

class article_repository {
public:
    explicit article_repository(postgres_conn_pool& pool);


    // 文章方法
    std::vector<article> get_all_published_aritcles();  // 获取所有文章
    std::optional<article> get_article_by_id(int id);   // 根据 id 获取文章
    int create_article(const article& article);         // 创建文章
    bool update_article(int id, const article& article);// 根据 id 更新文章
    bool delete_article(int id);                        // 根据 id 删除文章


    std::vector<article> get_articles_by_author(const std::string& author);     // 根据作者获取文章
    std::vector<article> search_articles(const std::string& keyword);           // 根据关键字获取文章

private:
    postgres_conn_pool& m_connection_pool;
    article map_row_to_article(const pqxx::row& row);    
};

#endif