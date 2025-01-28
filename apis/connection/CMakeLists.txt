cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

find_package(Boost REQUIRED COMPONENTS
             system
             filesystem
            )


add_library(connection
    STATIC
    connection.cpp
)

target_link_libraries(connection PRIVATE 
                      Boost::filesystem
                      )