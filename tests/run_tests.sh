#!/bin/bash
set -e

# Build the mock functions and tests
gcc -I tests/mocks -I src/urihandler \
    tests/mocks/mock_esp_http_server.c \
    src/urihandler/helper.c \
    tests/test_helper.c \
    -o tests/run_tests

# Run tests
tests/run_tests
