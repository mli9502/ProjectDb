cmake_minimum_required(VERSION 3.16)

set(HW_NAME main)

project(${HW_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Wall -O2")

include_directories(utils)

add_subdirectory(utils)
add_subdirectory(tests)

add_executable(${HW_NAME} main.cpp main.cpp benchmarks/bench.cpp)
target_link_libraries(${HW_NAME} PUBLIC utils pthread)

