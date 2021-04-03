cmake_minimum_required(VERSION 3.16)

project(projectdb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_library(db OBJECT
        key.cpp
        memtable.cpp
        memtable_queue.cpp
        projectdb.cpp
        sstable.cpp
        sstable_index.cpp
        sstable_index_queue.cpp
        sstable_ops.cpp
        table.cpp
        transaction_log.cpp
        value.cpp)

target_include_directories(db PRIVATE ${CMAKE_SOURCE_DIR}/config)
target_include_directories(db PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(db PRIVATE ${CMAKE_SOURCE_DIR}/utils)
