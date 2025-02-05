cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(qwq_apis)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMG_ROOT "/home/ppqwqqq/wallpaper" CACHE PATH "Path to image root.")
set(PDF_ROOT "/home/ppqwqqq/book" CACHE PATH "Path to pdf root.")


configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR})
 
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost REQUIRED COMPONENTS
             system
             filesystem
            )

find_package(nlohmann_json REQUIRED)

add_subdirectory(apis) 

add_executable(qwq_apis
    main.cpp
)



target_link_libraries(qwq_apis PRIVATE 
                      Boost::filesystem
                      )

target_link_libraries(qwq_apis 
    PRIVATE 
    pqxx
    db_utils
    connection router server
    articles
    users 
) 


