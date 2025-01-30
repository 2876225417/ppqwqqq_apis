cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

find_package(Boost REQUIRED COMPONENTS
             system
             filesystem
            )

find_package(OpenCV REQUIRED)

add_library(server
    STATIC
    server.cpp
)

target_link_libraries(server PRIVATE 
                      Boost::filesystem
                      ${OpenCVLIBS}
                      )

