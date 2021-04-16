cmake_minimum_required(VERSION 3.16)

# Add a custom Coverage build type:
# https://stackoverflow.com/questions/11437692/how-to-add-a-custom-build-type-to-cmake-targeting-make

set(LIB_NAME projectdb)

# https://stackoverflow.com/questions/1712756/g-and-c-compiler#:~:text=g%2B%2B%20is%20the%20gnu,to%20a%20non%20gcc%20compiler.

# gcovr cmake-build-coverage/ -r . --html --html-details -o details.html
# gcovr cmake-build-coverage/ -r . --xml -o coverage.xml
# https://gcovr.com/en/stable/guide.html
# https://stackoverflow.com/questions/43934862/how-to-use-gcovr-with-source-files-outside-the-current-build-run-directory

# Basic flags for code cov.
# https://gcovr.com/en/stable/guide.html
# set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage -fPIC -O0")
# Disable debug logging.
# string(APPEND CMAKE_CXX_FLAGS " -DNDEBUG")
# https://stackoverflow.com/questions/16433391/cmake-list-append-for-compiler-flags-yields-bogus-results
# https://stackoverflow.com/questions/9666800/getting-useful-gcov-results-for-header-only-libraries

project(${LIB_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Add a "Coverage" build type.
# https://gcovr.com/en/stable/guide.html
set(CMAKE_CXX_FLAGS_COVERAGE "-fprofile-arcs -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline -fPIC -O0 -DNDEBUG -g -Wall"
    CACHE STRING "Flags used for gen code coverage."
    FORCE)
mark_as_advanced(CMAKE_CXX_FLAGS_COVERAGE)
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
    CACHE STRING "Choose the type of build, options are: Debug Release Coverage."
    FORCE)

add_subdirectory(db)
add_subdirectory(config)
add_subdirectory(utils)

# For tests.
add_subdirectory(tests)

# For benchmark.
add_subdirectory(benchmark)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Wall -O2")
# The projectdb library.
# https://stackoverflow.com/questions/46944852/cmake-generate-static-library-based-on-other-static-libraries-and-packed-them-to
add_library(projectdb STATIC $<TARGET_OBJECTS:utils> $<TARGET_OBJECTS:config> $<TARGET_OBJECTS:db>)

# TODO: @mli: Probably should add sanitizers when buiding debug?

# Uncomment the following to add a main.cpp for testing.
# add_executable(main main.cpp)
# target_include_directories(main PRIVATE include)
# target_link_libraries(main PUBLIC projectdb pthread)
