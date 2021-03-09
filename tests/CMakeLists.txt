cmake_minimum_required(VERSION 3.16)

project(tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)

add_executable(tests main_tests.cpp serializer_tests.cpp)

target_link_libraries(tests PUBLIC utils gtest gtest_main pthread)
