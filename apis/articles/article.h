#ifndef ARTICLE_H
#define ARTICLE_H

#include <chrono>
#include <string>


struct article {
    int id;
    std::string title;
    std::string content;
    std::string author;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    bool published;

    static std::chrono::system_clock::time_point from_pg_timestamp(const std::string& pg_time);
    static std::string to_pg_timestamp(const std::chrono::system_clock::time_point& tp);
};

#endif