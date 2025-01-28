cmake_minimum_required(VERSION 3.28 FATAL_ERROR)



add_library(db_utils
    STATIC
    postgres_conn.cpp
    conn_pool.cpp
)

target_link_libraries(db_utils PRIVATE pqxx) 