cmake_minimum_required(VERSION 3.16)

project(config LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_library(config OBJECT db_config.cpp)
