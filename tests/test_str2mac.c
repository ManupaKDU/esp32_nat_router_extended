#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "helper.h"

// Mock functions required by helper.c but not used by str2mac
int httpd_query_key_value(char *parameterString, char *parameter, char *buffer, size_t paramLength) { return 0; }
int httpd_req_recv(httpd_req_t *r, char *buf, size_t len) { return 0; }

void test_str2mac() {
    // Happy paths
    assert(str2mac("00:11:22:33:44:55") == true);
    assert(str2mac("aa:bb:cc:dd:ee:ff") == true);
    assert(str2mac("AA:BB:CC:DD:EE:FF") == true);
    assert(str2mac("12:34:56:78:90:ab") == true);

    // Invalid formats
    assert(str2mac("00-11-22-33-44-55") == false);
    assert(str2mac("001122334455") == false);
    assert(str2mac("00:11:22:33:44") == false);
    assert(str2mac("00:11:22:33:44:55:66") == false);

    // Invalid characters
    assert(str2mac("G0:11:22:33:44:55") == false);
    assert(str2mac("00:11:22:33:44:5z") == false);

    // Edge cases
    assert(str2mac("") == false);
    assert(str2mac(NULL) == false);
    assert(str2mac(":::::") == false);
    assert(str2mac("abc") == false);

    printf("All str2mac tests passed!\n");
}

int main() {
    test_str2mac();
    return 0;
}
