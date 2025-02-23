cmake_minimum_required(VERSION 3.26)
project(lab2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(random_cache SHARED random_cache.cpp)

target_include_directories(random_cache PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(lab2 main.cpp dedup.cpp)
target_link_libraries(lab2 PRIVATE random_cache)
