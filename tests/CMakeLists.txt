cmake_minimum_required(VERSION 3.16)

project(tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/config)
include_directories(${CMAKE_SOURCE_DIR}/db)
include_directories(${CMAKE_SOURCE_DIR}/utils)

add_executable(tests test_utils.cpp smoke_tests.cpp serializer_tests.cpp memtable_tests.cpp)

target_link_libraries(tests PUBLIC utils config db gtest gtest_main pthread)
