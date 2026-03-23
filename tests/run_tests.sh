#!/bin/bash
set -e

echo "Compiling tests..."
gcc -I./tests/mocks -I./tests/mocks/sys -I./tests/mocks/lwip -I./src/urihandler \
    tests/test_helper.c src/urihandler/helper.c \
    -o tests/run_tests

echo "Running tests..."
./tests/run_tests
