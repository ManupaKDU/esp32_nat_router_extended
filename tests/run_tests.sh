#!/bin/bash
gcc -DHOST_TESTING -I./tests/mocks -I./src/urihandler -Dbool=_Bool -include stdbool.h -include string.h -include stdlib.h -include stdint.h tests/test_helper.c src/urihandler/helper.c -o tests/test_helper
./tests/test_helper

gcc -DHOST_TESTING -I./tests/mocks -I./components/cmd_nvs -I./src/urihandler -Dbool=_Bool -include stdbool.h -include string.h -include stdlib.h -include stdint.h tests/test_cmd_nvs.c -o tests/test_cmd_nvs
./tests/test_cmd_nvs
