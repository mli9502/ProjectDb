cmake_minimum_required(VERSION 3.16)

project(tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(benchmark benchmark.cpp bench.cpp)

target_link_libraries(benchmark PUBLIC utils config db pthread)
