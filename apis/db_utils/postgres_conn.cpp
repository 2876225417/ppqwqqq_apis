

#include "postgres_conn.h"

postgres_conn::postgres_conn(const std::string& conn_str) {
    try {
        m_conn = std::make_unique<pqxx::connection>(conn_str);
        if (!m_conn->is_open())
            throw std::runtime_error("Failed to open database connection!");
    } catch (const pqxx::sql_error& e) {
        throw std::runtime_error("SQL error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw std::runtime_error("Connection error: " + std::string(e.what()));
    }
}

postgres_conn::~postgres_conn() {
    if (m_conn && m_conn->is_open())
        m_conn->close();
}

pqxx::work postgres_conn::begin_transaction() {
    return pqxx::work(*m_conn);
}

pqxx::result postgres_conn::execute(const std::string& query) {
    try {
        pqxx::nontransaction nt(*m_conn);
        return nt.exec(query);
    } catch (const pqxx::sql_error& e) {
        throw std::runtime_error("Query failed: " + std::string(e.what()));
    }
}

bool postgres_conn::is_valid() const {
    return m_conn && m_conn->is_open();
}

