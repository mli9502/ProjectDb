cmake_minimum_required(VERSION 3.16)

set(LIB_NAME projectdb)

project(${LIB_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Wall -O2")

add_subdirectory(db)
add_subdirectory(config)
add_subdirectory(utils)

# For tests.
add_subdirectory(tests)

# For benchmark.
add_subdirectory(benchmark)

# The projectdb library.
# https://stackoverflow.com/questions/46944852/cmake-generate-static-library-based-on-other-static-libraries-and-packed-them-to
add_library(projectdb STATIC $<TARGET_OBJECTS:utils> $<TARGET_OBJECTS:config> $<TARGET_OBJECTS:db>)

# Uncomment the following to add a main.cpp for testing.
# add_executable(main main.cpp)
# target_include_directories(main PRIVATE include)
# target_link_libraries(main PUBLIC projectdb pthread)
