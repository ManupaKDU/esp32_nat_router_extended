#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_http_server.h"

// Functions to test
extern void preprocess_string(char *str);
extern void readUrlParameterIntoBuffer(char *parameterString, char *parameter, char *buffer, size_t paramLength);
extern esp_err_t fill_post_buffer(httpd_req_t *req, char *buf, size_t len);
extern bool is_valid_subnet_mask(char *subnet_mask);

// Mock setter
extern void set_mock_req_recv_data(char *data, size_t len, int ret_val, bool timeout);

void test_preprocess_string() {
    printf("Testing preprocess_string...\n");

    char str1[] = "hello+world";
    preprocess_string(str1);
    assert(strcmp(str1, "hello world") == 0);

    char str2[] = "test%20space";
    preprocess_string(str2);
    assert(strcmp(str2, "test space") == 0);

    char str3[] = "a%2Bb";
    preprocess_string(str3);
    assert(strcmp(str3, "a+b") == 0);

    char str4[] = "%41%42%43";
    preprocess_string(str4);
    assert(strcmp(str4, "ABC") == 0);

    // Invalid/incomplete hex
    char str5[] = "test%";
    preprocess_string(str5);
    // Note: implementation doesn't touch it if *(p+1) or *(p+2) is 0
    assert(strcmp(str5, "test%") == 0);

    char str6[] = "test%2";
    preprocess_string(str6);
    assert(strcmp(str6, "test%2") == 0);

    printf("test_preprocess_string passed.\n");
}

void test_readUrlParameterIntoBuffer() {
    printf("Testing readUrlParameterIntoBuffer...\n");

    char buffer[100];

    readUrlParameterIntoBuffer("param1=value1&param2=hello+world", "param2", buffer, sizeof(buffer));
    assert(strcmp(buffer, "hello world") == 0);

    readUrlParameterIntoBuffer("param1=value1", "param2", buffer, sizeof(buffer));
    assert(strlen(buffer) == 0); // Not found -> empty string

    readUrlParameterIntoBuffer("param1=test%20encoded", "param1", buffer, sizeof(buffer));
    assert(strcmp(buffer, "test encoded") == 0);

    printf("test_readUrlParameterIntoBuffer passed.\n");
}

void test_fill_post_buffer() {
    printf("Testing fill_post_buffer...\n");

    char buffer[100];
    httpd_req_t req;

    // Test successful read
    char *test_data = "post data";
    set_mock_req_recv_data(test_data, strlen(test_data), 0, false);
    esp_err_t err = fill_post_buffer(&req, buffer, strlen(test_data));
    assert(err == ESP_OK);
    assert(strcmp(buffer, "post data") == 0);

    // Test with timeout once then success
    set_mock_req_recv_data(test_data, strlen(test_data), 0, true);
    err = fill_post_buffer(&req, buffer, strlen(test_data));
    assert(err == ESP_OK);
    assert(strcmp(buffer, "post data") == 0);

    // Test read failure
    set_mock_req_recv_data(NULL, 0, ESP_FAIL, false);
    err = fill_post_buffer(&req, buffer, 10);
    assert(err == ESP_FAIL);

    printf("test_fill_post_buffer passed.\n");
}

void test_is_valid_subnet_mask() {
    printf("Testing is_valid_subnet_mask...\n");

    // Valid masks
    assert(is_valid_subnet_mask("255.255.255.0") == true);
    assert(is_valid_subnet_mask("255.255.0.0") == true);
    assert(is_valid_subnet_mask("255.0.0.0") == true);
    assert(is_valid_subnet_mask("255.255.255.255") == true);
    assert(is_valid_subnet_mask("128.0.0.0") == true);
    assert(is_valid_subnet_mask("255.255.255.128") == true);
    assert(is_valid_subnet_mask("255.255.255.192") == true);
    assert(is_valid_subnet_mask("255.255.255.252") == true);

    // Invalid masks
    assert(is_valid_subnet_mask("255.255.255") == false); // Not 4 octets
    assert(is_valid_subnet_mask("255.255.255.0.0") == false); // 5 octets
    assert(is_valid_subnet_mask("255.255.256.0") == false); // Out of range
    assert(is_valid_subnet_mask("-1.255.255.0") == false); // Out of range
    assert(is_valid_subnet_mask("255.255.255.1") == false); // Invalid bit pattern
    assert(is_valid_subnet_mask("255.255.0.255") == false); // Invalid bit pattern
    assert(is_valid_subnet_mask("255.0.255.0") == false); // Invalid bit pattern
    assert(is_valid_subnet_mask("255.255.255.127") == false); // Invalid bit pattern

    printf("test_is_valid_subnet_mask passed.\n");
}

int main() {
    test_preprocess_string();
    test_readUrlParameterIntoBuffer();
    test_fill_post_buffer();
    test_is_valid_subnet_mask();

    printf("All tests passed!\n");
    return 0;
}
