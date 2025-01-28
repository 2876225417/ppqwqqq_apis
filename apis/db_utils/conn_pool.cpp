

#include "conn_pool.h"
#include <iostream>


void postgres_conn_pool::configure(
    const std::string& db_name,
    const std::string& db_user,
    const std::string& db_password,
    const std::string& db_host,
    int db_port,
    size_t pool_size
) {
    std::lock_guard<std::mutex> lock(m_pool_mutex);
    m_conn_str = 
        "postgresql://" + db_user + ":" + db_password + 
        "@" + db_host  + ":" + std::to_string(db_port) +
        "/" + db_name;

    m_max_pool_size = pool_size;
    
    while (m_pool.size() < m_max_pool_size) {
        try {
            auto conn = std::make_shared<postgres_conn>(m_conn_str);
            m_pool.push(conn);
        } catch (...) {
            break;
        }
    }
}

std::shared_ptr<postgres_conn> 
postgres_conn_pool::acquire() {
    std::unique_lock<std::mutex> lock(m_pool_mutex);
    m_pool_cv.wait(lock, [this] { return !m_pool.empty(); });
    
    auto conn = m_pool.front();
    m_pool.pop();

    return conn;
}

void postgres_conn_pool::release(std::shared_ptr<postgres_conn> conn) {
    std::lock_guard<std::mutex> lock(m_pool_mutex);
    if (conn->is_valid() && m_pool.size() < m_max_pool_size) 
        m_pool.push(conn);

    m_pool_cv.notify_one();
}