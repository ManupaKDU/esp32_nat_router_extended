#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "helper.h"

// Simple testing macro
#define TEST_ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } else { \
        printf("PASS: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    } \
} while (0)

// Dummy implementations for mocked functions
int httpd_query_key_value(const char *qry, const char *key, char *val, size_t val_size) {
    return ESP_FAIL;
}

int httpd_req_recv(httpd_req_t *r, char *buf, size_t buf_len) {
    return -1;
}

// Wrapper to prevent segfaults when passing string literals to functions expecting char*
bool check_subnet(const char *subnet) {
    char buf[128];
    strncpy(buf, subnet, sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';
    return is_valid_subnet_mask(buf);
}

void test_is_valid_subnet_mask() {
    printf("Testing is_valid_subnet_mask...\n");

    // Valid subnet masks
    TEST_ASSERT(check_subnet("255.255.255.0") == true);
    TEST_ASSERT(check_subnet("255.255.255.255") == true);
    TEST_ASSERT(check_subnet("255.255.255.128") == true);
    TEST_ASSERT(check_subnet("255.255.255.192") == true);
    TEST_ASSERT(check_subnet("255.255.255.224") == true);
    TEST_ASSERT(check_subnet("255.255.255.240") == true);
    TEST_ASSERT(check_subnet("255.255.255.248") == true);
    TEST_ASSERT(check_subnet("255.255.255.252") == true);
    TEST_ASSERT(check_subnet("255.255.255.254") == true);
    TEST_ASSERT(check_subnet("255.255.0.0") == true);
    TEST_ASSERT(check_subnet("255.0.0.0") == true);
    TEST_ASSERT(check_subnet("0.0.0.0") == true);

    // Invalid subnet masks (incorrect format)
    TEST_ASSERT(check_subnet("255.255.255") == false);
    TEST_ASSERT(check_subnet("255.255.255.0.0") == false);
    TEST_ASSERT(check_subnet("abc.def.ghi.jkl") == false);
    TEST_ASSERT(check_subnet("255.255.255.") == false);
    TEST_ASSERT(check_subnet("...0") == false);

    // Invalid subnet masks (out of range octets)
    TEST_ASSERT(check_subnet("256.255.255.0") == false);
    TEST_ASSERT(check_subnet("255.255.255.-1") == false);

    // Invalid subnet masks (discontiguous ones)
    TEST_ASSERT(check_subnet("255.255.255.1") == false);
    TEST_ASSERT(check_subnet("255.255.0.255") == false);
    TEST_ASSERT(check_subnet("255.0.255.0") == false);
    TEST_ASSERT(check_subnet("0.255.255.0") == false);
    TEST_ASSERT(check_subnet("255.255.255.127") == false);
    TEST_ASSERT(check_subnet("255.255.128.0") == true); // Valid
    TEST_ASSERT(check_subnet("255.255.127.0") == false);

    printf("All is_valid_subnet_mask tests passed!\n");
}

int main() {
    test_is_valid_subnet_mask();
    return 0;
}
