cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

find_package(OpenCV REQUIRED)

add_library(images
    STATIC
    image.cpp
)

target_link_libraries(images PRIVATE ${OpenCVLIBS})
target_include_directories(images PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
 