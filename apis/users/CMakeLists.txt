cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

find_package(Boost REQUIRED COMPONENTS
             system
             filesystem
            )

find_package(OpenSSL REQUIRED)


add_library(users
    STATIC
    user_management.cpp
)

target_link_libraries(users PRIVATE 
                      Boost::filesystem
                      OpenSSL::Crypto
                      )