cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost REQUIRED COMPONENTS
             system
             filesystem
            )



add_library(server
    STATIC
    server.cpp
)


target_link_libraries(server PRIVATE 
                      Boost::filesystem
                      )

