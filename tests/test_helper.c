#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "helper.h"

// Mock definitions for other functions in helper.c to compile
int httpd_query_key_value(const char *qry, const char *key, char *val, size_t val_size) { return 0; }
int httpd_req_recv(httpd_req_t *r, char *buf, size_t buf_len) { return 0; }

void test_preprocess_string() {
    printf("Running test_preprocess_string...\n");

    // Test 1: Empty string
    char str1[] = "";
    preprocess_string(str1);
    assert(strcmp(str1, "") == 0);

    // Test 2: Normal string without special characters
    char str2[] = "hello";
    preprocess_string(str2);
    assert(strcmp(str2, "hello") == 0);

    // Test 3: + decoding to space
    char str3[] = "hello+world";
    preprocess_string(str3);
    assert(strcmp(str3, "hello world") == 0);

    // Test 4: URL decoding %20 -> space
    char str4[] = "hello%20world";
    preprocess_string(str4);
    assert(strcmp(str4, "hello world") == 0);

    // Test 5: URL decoding other characters
    char str5[] = "%21%40%23%24%25%5E%26%2A%28%29";
    preprocess_string(str5);
    assert(strcmp(str5, "!@#$%^&*()") == 0);

    // Test 6: Incomplete % encoding at end
    char str6[] = "hello%";
    preprocess_string(str6);
    assert(strcmp(str6, "hello%") == 0);

    char str7[] = "hello%2";
    preprocess_string(str7);
    assert(strcmp(str7, "hello%2") == 0);

    // Test 7: Combination
    char str8[] = "name=John+Doe&age=30%21";
    preprocess_string(str8);
    assert(strcmp(str8, "name=John Doe&age=30!") == 0);

    // Test 8: Lowercase hex vs Uppercase hex
    char str9[] = "%2a%2B";
    preprocess_string(str9);
    assert(strcmp(str9, "*+") == 0);

    printf("All test_preprocess_string passed!\n");
}

void test_str2mac() {
    printf("Running test_str2mac...\n");
    uint8_t values[6];

    // Test 1: Valid uppercase MAC
    assert(str2mac("00:1A:2B:3C:4D:5E", values) == true);
    assert(values[0] == 0x00 && values[1] == 0x1a && values[2] == 0x2b && values[3] == 0x3c && values[4] == 0x4d && values[5] == 0x5e);

    // Test 2: Valid lowercase MAC
    assert(str2mac("a1:b2:c3:d4:e5:f6", values) == true);
    assert(values[0] == 0xa1 && values[1] == 0xb2 && values[2] == 0xc3 && values[3] == 0xd4 && values[4] == 0xe5 && values[5] == 0xf6);

    // Test 3: Invalid length
    assert(str2mac("00:1A:2B:3C:4D:5", values) == false);
    assert(str2mac("00:1A:2B:3C:4D:5E:6F", values) == false);

    // Test 4: Invalid characters
    assert(str2mac("00:1A:2B:3C:4D:5G", values) == false);
    assert(str2mac("00:1A:2B:3C:4D:-E", values) == false);

    // Test 5: Missing colons
    assert(str2mac("0011A:2B:3C:4D:5E", values) == false);
    assert(str2mac("00:1A:2B3C:4D:5E:", values) == false);

    printf("All test_str2mac passed!\n");
}

int main() {
    test_preprocess_string();
    test_str2mac();
    return 0;
}
