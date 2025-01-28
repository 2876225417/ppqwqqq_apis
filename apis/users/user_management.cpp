

#include "user_management.h"
#include <random>
#include <algorithm>


user_manager::user_manager(postgres_conn_pool& pool)
    : m_connection_pool(pool) { }

std::string user_manager::generate_salt() const {
    constexpr int SALT_LENGTH = 16;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(33, 126);

    std::string salt;
    salt.reserve(SALT_LENGTH);
    for (int i = 0; i < SALT_LENGTH; ++i)
        salt += static_cast<char>(dis(gen));

    return salt;
}

std::string user_manager::hash_password(const std::string& password, const std::string& salt) const {
    std::string salted_password = password + salt;

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256( reinterpret_cast<const unsigned char*>(salted_password.c_str())
          , salted_password.size()
          , hash
          ) ;

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) 
          ss << std::hex
             << std::setw(2)
             << std::setfill('0')
             << static_cast<int>(hash[i]);
    return ss.str();
}

#include <iostream>

std::optional<user_info> 
user_manager::user_register( const std::string& user_name
                     , const std::string& user_password
                     ) {
    auto conn = m_connection_pool.acquire();
    try {
        
        pqxx::work tx(conn->get_raw_connection());
        tx.conn().prepare(
            "check_exist_user",
            "SELECT user_id FROM users WHERE username = $1"
        );
        auto result = tx.exec_prepared("check_exist_user", user_name);
        if (!result.empty()) {
            m_connection_pool.release(conn);
            return std::nullopt;
        }
        std::string salt = generate_salt();
        std::string password_hash = hash_password(user_password, salt);
        tx.conn().prepare(
            "register",
            "INSERT INTO users (username, password_hash, salt) VALUES ($1, $2, $3) RETURNING user_id"
        );
        tx.commit();

        auto insert_result = tx.exec_prepared("register", user_name, password_hash, salt);
        m_connection_pool.release(conn);

        return user_info {
            insert_result[0][0].as<int>(),
            user_name,
            password_hash,
            salt
        };

    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        m_connection_pool.release(conn);
        return std::nullopt;
    }

    m_connection_pool.release(conn);
    return std::nullopt;
}

std::optional<user_info> user_manager::user_login( const std::string& user_name
                                              , const std::string& user_password
                                              ) {
    auto conn = m_connection_pool.acquire();
    try {
        pqxx::work tx(conn->get_raw_connection());
        tx.conn().prepare(
            "sign_in",
            "SELECT user_id, password_hash, salt FROM users WHERE username = $1"
        );
        auto result = tx.exec_prepared("sign_in", user_name);
        if (result.empty()) {
            m_connection_pool.release(conn);
            return std::nullopt;
        }
        tx.commit();
        auto row = result[0];
        std::string stored_hash = row[1].as<std::string>();
        std::string salt = row[2].as<std::string>();

        std::string input_hash = hash_password(user_password, salt);
        if (input_hash != stored_hash) {
            m_connection_pool.release(conn);
            return std::nullopt;
        }
        
        return user_info {
            row[0].as<int>(),
            user_name,
            stored_hash,
            salt
        };

    } catch (const std::exception& e) {
        m_connection_pool.release(conn);
        return std::nullopt;
    }
    m_connection_pool.release(conn);
    return std::nullopt;
}