cmake_minimum_required(VERSION 3.16)

project(utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(utils OBJECT
        db_concepts.cpp
        exception.cpp
        log.cpp
        serializer.cpp
        system_utils.cpp)

target_include_directories(utils PRIVATE ${CMAKE_SOURCE_DIR}/config)
