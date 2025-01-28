cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(qwq_apis)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
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

target_link_libraries(qwq_apis PRIVATE pqxx) 
target_link_libraries(qwq_apis PRIVATE db_utils)
target_link_libraries(qwq_apis PRIVATE connection router server)
target_link_libraries(qwq_apis PRIVATE articles)
target_link_libraries(qwq_apis PRIVATE users)
