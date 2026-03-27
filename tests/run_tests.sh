#!/bin/bash
set -e

gcc -I./tests/mocks -I./src/urihandler \
    src/urihandler/helper.c \
    tests/test_str2mac.c \
    -o tests/test_runner

./tests/test_runner
rm tests/test_runner
