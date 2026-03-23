#!/bin/bash
gcc -I./tests/mocks -I./src/urihandler -Dbool=_Bool -include stdbool.h -include string.h -include stdlib.h tests/test_helper.c src/urihandler/helper.c -o tests/test_helper
./tests/test_helper
