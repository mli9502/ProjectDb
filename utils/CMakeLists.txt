cmake_minimum_required(VERSION 3.16)

project(utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(utils STATIC log.cpp exception.cpp memtable.cpp key.cpp value.cpp db_concepts.cpp serializer.cpp memtable_queue.cpp sstable_index.cpp db_config.cpp sstable.cpp sstable.h table.cpp table.h)
