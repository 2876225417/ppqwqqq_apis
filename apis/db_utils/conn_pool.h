
#ifndef CONN_POOL_H
#define CONN_POOL_H

#include <memory>
#include <mutex>
#include <condition_variable>
#include <pqxx/pqxx>

#include "postgres_conn.h"


class postgres_conn_pool {
    public:
        static postgres_conn_pool& instance() {
            static postgres_conn_pool instance;
            return instance;
        }
    
        std::shared_ptr<postgres_conn> acquire();
        void release(std::shared_ptr<postgres_conn> conn);
        
        void configure( const std::string& db_name
                      , const std::string& db_user
                      , const std::string& password
                      , const std::string& host
                      , int port
                      , size_t pool_size = 10
                      ) ;
    private:
        postgres_conn_pool() = default;
    
        std::queue<std::shared_ptr<postgres_conn>>  m_pool;
        std::mutex                                  m_pool_mutex;
        std::condition_variable                     m_pool_cv;
        size_t                                      m_max_pool_size = 10;
        std::string                                 m_conn_str;
};


#endif