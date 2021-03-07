cmake_minimum_required(VERSION 3.16)

project(utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(utils STATIC log.cpp exception.cpp)