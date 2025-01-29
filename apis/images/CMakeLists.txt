cmake_minimum_required(VERSION 3.28 FATAL_ERROR)



find_package(OpenCV REQUIRED)

add_library(image_resolver
    STATIC
    image_resolver.cpp
)

target_link_directories(image_resolver PRIVATE ${OpenCV_LIBS})

