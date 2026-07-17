#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "helper.h"

// Mock definitions for other functions in helper.c to compile
int httpd_query_key_value(const char *qry, const char *key, char *val, size_t val_size) {
    if (qry == NULL || key == NULL || val == NULL || val_size == 0) return ESP_FAIL;

    const char *p = qry;
    size_t key_len = strlen(key);

    while (*p) {
        if (strncmp(p, key, key_len) == 0 && p[key_len] == '=') {
            const char *start = p + key_len + 1;
            const char *end = strchr(start, '&');
            size_t len = end ? (size_t)(end - start) : strlen(start);

            if (len >= val_size) {
                return ESP_FAIL; // Buffer too small
            }
            memcpy(val, start, len);
            val[len] = '\0';
            return ESP_OK;
        }
        p = strchr(p, '&');
        if (!p) break;
        p++; // skip '&'
    }
    return ESP_FAIL;
}
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

void test_readUrlParameterIntoBuffer() {
    printf("Running test_readUrlParameterIntoBuffer...\n");
    char buffer[64];

    // Test 1: Standard parameter
    readUrlParameterIntoBuffer("name=John+Doe&age=30", "name", buffer, sizeof(buffer));
    assert(strcmp(buffer, "John Doe") == 0);

    // Test 2: Parameter with URL encoding
    readUrlParameterIntoBuffer("city=New+York%21", "city", buffer, sizeof(buffer));
    assert(strcmp(buffer, "New York!") == 0);

    // Test 3: Sensitive parameter (pass) - logging should be redacted but buffer should contain the value
    readUrlParameterIntoBuffer("pass=secret123", "pass", buffer, sizeof(buffer));
    assert(strcmp(buffer, "secret123") == 0);

    // Test 4: Sensitive parameter (user)
    readUrlParameterIntoBuffer("user=admin", "user", buffer, sizeof(buffer));
    assert(strcmp(buffer, "admin") == 0);

    // Test 5: Parameter not found
    readUrlParameterIntoBuffer("name=John", "age", buffer, sizeof(buffer));
    assert(strcmp(buffer, "") == 0);

    // Test 6: Buffer too small for the value
    readUrlParameterIntoBuffer("name=JohnDoe", "name", buffer, 5);
    // Based on our mock, it returns ESP_FAIL if len >= val_size, so readUrlParameterIntoBuffer should set buffer[0] = '\0'
    assert(strcmp(buffer, "") == 0);

    // Test 7: Empty value
    readUrlParameterIntoBuffer("name=&age=30", "name", buffer, sizeof(buffer));
    assert(strcmp(buffer, "") == 0);

    printf("All test_readUrlParameterIntoBuffer passed!\n");
}

void test_sanitize_html() {
    printf("Running test_sanitize_html...\n");
    char buffer[128];

    // Test 1: NULL inputs and zero length
    sanitize_html(NULL, buffer, sizeof(buffer)); // Should not crash
    sanitize_html("test", NULL, sizeof(buffer)); // Should not crash
    buffer[0] = 'x';
    sanitize_html("test", buffer, 0); // Should not modify
    assert(buffer[0] == 'x');

    // Test 2: Normal string
    sanitize_html("hello world", buffer, sizeof(buffer));
    assert(strcmp(buffer, "hello world") == 0);

    // Test 3: Special characters < > & " '
    sanitize_html("<script>alert('xss' & \"test\")</script>", buffer, sizeof(buffer));
    assert(strcmp(buffer, "&lt;script&gt;alert(&#39;xss&#39; &amp; &quot;test&quot;)&lt;/script&gt;") == 0);

    // Test 4: Buffer limit behavior (normal chars)
    char small_buf[5];
    sanitize_html("12345", small_buf, sizeof(small_buf));
    assert(strcmp(small_buf, "1234") == 0);

    // Test 5: Buffer limit behavior (truncated tag)
    char small_buf2[8];
    // "&lt;" is 4 chars, 'a' is 1. We need room for null.
    sanitize_html("<ab", small_buf2, sizeof(small_buf2));
    // Length 8: 0 to 6 are chars, 7 is null.
    // '<' -> "&lt;" (4 bytes). Next is 'a' (1 byte). Next is 'b' (1 byte). Total 6 bytes + null = 7 bytes.
    assert(strcmp(small_buf2, "&lt;ab") == 0);

    // Test 6: Buffer limit behavior (not enough room for full tag)
    char small_buf3[5];
    sanitize_html("a<b", small_buf3, sizeof(small_buf3));
    // 'a' (1). '<' needs 4 bytes. Total 5 + 1 = 6 bytes needed.
    // Capacity 5, out_idx=0. 'a' -> out_idx=1.
    // next is '<', out_idx + 4 = 1 + 4 = 5 >= output_size (5). Break.
    // Output: "a\0"
    assert(strcmp(small_buf3, "a") == 0);

    printf("All test_sanitize_html passed!\n");
}

int main() {
    test_preprocess_string();
    test_str2mac();
    test_readUrlParameterIntoBuffer();
    test_sanitize_html();
    return 0;
}
