#!/bin/bash

case $1 in
  test)
    printf "Running tests...\n"
    make clean && make init_build && make run_tests
    # Exit with whatever exit code make produce.
    # This will reflect test failure if not 0.
    exit $?
    ;;
  coverage)
    printf "Running coverage...\n"
    make clean && make BUILD=coverage init_build && make coverage
    # ../projectdb is the directory that we mount to docker during "docker run" cmd.
    # This is needed so that we can "copy" the generated report out of container.
    # https://stackoverflow.com/questions/57846627/copy-a-file-from-docker-container-to-host
    # docker run -it --rm -v ${PWD}:/projectdb projectdb coverage
    cp ./docs/coverage.html ../projectdb/docs/coverage.html
    ;;
  *)
    printf "Found unknown option! Valid options are: [test, coverage]\n"
    exit 1
    ;;
esac
