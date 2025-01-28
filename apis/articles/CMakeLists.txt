cmake_minimum_required(VERSION 3.28 FATAL_ERROR)


find_package(nlohmann_json REQUIRED)

add_library(articles
    STATIC
    article_repository.cpp
)

target_link_libraries(articles PRIVATE db_utils)
