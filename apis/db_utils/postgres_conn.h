


#ifndef POSTGRES_CONN_H
#define POSTGRES_CONN_H

#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

class postgres_conn_pool;

class postgres_conn {
public:
    postgres_conn(const std::string& conn_str);
    ~postgres_conn();

    pqxx::work begin_transaction();
    pqxx::result execute(const std::string& query);
    bool is_valid() const;

    pqxx::connection& get_raw_connection() {
        return *m_conn;
    }

private:
    friend class postgres_conn_pool;
    std::unique_ptr<pqxx::connection> m_conn;
};

#endif