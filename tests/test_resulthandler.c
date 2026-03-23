#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Mock functions and definitions required to compile resulthandler.c
typedef int esp_err_t;
typedef struct httpd_req httpd_req_t;
typedef int httpd_err_code_t;
#define HTTPD_RESP_USE_STRLEN -1

#define DEFAULT_SCAN_LIST_SIZE 10
#define PARAM_NAMESPACE "mock"
#define NVS_READWRITE 1
typedef int nvs_handle_t;

void get_config_param_str(char* name, char** param) { *param = NULL; }
void get_config_param_int(char* name, int32_t* param) { *param = 0; }
int nvs_open(const char* name, int mode, nvs_handle_t *out_handle) { return 0; }
int nvs_set_i32(nvs_handle_t handle, const char* key, int32_t value) { return 0; }
int nvs_commit(nvs_handle_t handle) { return 0; }
void nvs_close(nvs_handle_t handle) {}
void httpd_req_to_sockfd(httpd_req_t *req) {}
void closeHeader(httpd_req_t *req) {}
int httpd_resp_send(httpd_req_t *req, const char *buf, ssize_t buf_len) { return 0; }
bool isLocked() { return false; }
int redirectToLock(httpd_req_t *req) { return 0; }

#define ESP_LOGI(...)

// Extern variables required by resulthandler.c
const char result_start[] = "start";
const char result_end[] = "start_end";

// Instead of including handler.h, we declare the function we want to test
char *findTextColorForSSID(int8_t rssi);

// Now include the source file to test
#include "resulthandler.c"

int main() {
    printf("Running findTextColorForSSID tests...\n");

    // Test success case (rssi >= -50)
    assert(strcmp(findTextColorForSSID(0), "success") == 0);
    assert(strcmp(findTextColorForSSID(-20), "success") == 0);
    assert(strcmp(findTextColorForSSID(-50), "success") == 0);

    // Test info case (-70 <= rssi < -50)
    assert(strcmp(findTextColorForSSID(-51), "info") == 0);
    assert(strcmp(findTextColorForSSID(-60), "info") == 0);
    assert(strcmp(findTextColorForSSID(-70), "info") == 0);

    // Test warning case (rssi < -70)
    assert(strcmp(findTextColorForSSID(-71), "warning") == 0);
    assert(strcmp(findTextColorForSSID(-80), "warning") == 0);
    assert(strcmp(findTextColorForSSID(-100), "warning") == 0);
    assert(strcmp(findTextColorForSSID(-128), "warning") == 0);

    printf("All tests passed!\n");
    return 0;
}
