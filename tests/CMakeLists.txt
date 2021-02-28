cmake_minimum_required(VERSION 3.16)

project(tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(tests main_tests.cpp)

target_link_libraries(tests PUBLIC gtest gtest_main pthread)
