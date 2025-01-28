


#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H


#include "../db_utils/conn_pool.h"
#include <string>
#include <pqxx/pqxx>
#include <optional>
#include <openssl/sha.h>

struct user_info {
    int user_id;
    std::string user_name;
    std::string password_hash;
    std::string salt;
};

class user_manager {
public:
    explicit user_manager(postgres_conn_pool& pool);

    // 用户管理
    // --sign in
    std::optional<user_info> user_register(const std::string& user_name, const std::string& password);
    // --sign up
    std::optional<user_info> user_login(const std::string& user_name, const std::string& password);

private:
    std::string generate_salt() const;
    std::string hash_password(const std::string& password, const std::string& salt) const;

    postgres_conn_pool& m_connection_pool;
};

#endif