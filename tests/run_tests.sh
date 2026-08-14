#!/bin/bash
set -e

gcc -DHOST_TESTING -I./tests/mocks -I./src/urihandler -Dbool=_Bool -include stdbool.h -include string.h -include stdlib.h -include stdint.h tests/test_helper.c src/urihandler/helper.c -o tests/test_helper
./tests/test_helper

gcc -DHOST_TESTING -I./tests/mocks -I./components/cmd_nvs -I./src/urihandler -Dbool=_Bool -include stdbool.h -include string.h -include stdlib.h -include stdint.h tests/test_cmd_nvs.c -o tests/test_cmd_nvs
./tests/test_cmd_nvs

# Test lockhandler
sed '/#include/d' src/urihandler/lockhandler.c > tests/lockhandler_stripped.c
gcc -DHOST_TESTING -I./tests/mocks -Dbool=_Bool -include stdbool.h -include string.h -include stdlib.h -include stdint.h tests/test_lockhandler.c -o tests/test_lockhandler
./tests/test_lockhandler
