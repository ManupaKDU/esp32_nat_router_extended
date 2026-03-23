#!/bin/bash
set -e

# Compile the test using host GCC
gcc -o tests/test_resulthandler tests/test_resulthandler.c -I src/urihandler -I tests/mocks

# Run the compiled test
./tests/test_resulthandler
