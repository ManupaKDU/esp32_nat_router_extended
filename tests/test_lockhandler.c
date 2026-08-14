#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef int esp_err_t;

struct httpd_req {
    size_t content_len;
    int method;
};
typedef struct httpd_req httpd_req_t;
typedef int httpd_err_code_t;
#define HTTPD_RESP_USE_STRLEN -1

#define PARAM_NAMESPACE "mock"
#define NVS_READWRITE 1
typedef int nvs_handle_t;

void get_config_param_str(char* name, char** param) { *param = NULL; }
void get_config_param_int(char* name, int32_t* param) { *param = 0; }
int nvs_open(const char* name, int mode, nvs_handle_t *out_handle) { return 0; }
int nvs_set_str(nvs_handle_t handle, const char* key, const char* value) { return 0; }
int nvs_commit(nvs_handle_t handle) { return 0; }
void nvs_close(nvs_handle_t handle) {}
void httpd_req_to_sockfd(httpd_req_t *req) {}
void closeHeader(httpd_req_t *req) {}
int httpd_resp_send(httpd_req_t *req, const char *buf, ssize_t buf_len) { return 0; }

esp_err_t fill_post_buffer(httpd_req_t *req, char *buf, size_t len) { return 0; }
void readUrlParameterIntoBuffer(char *buf, const char *parameter, char *buffer, size_t paramLength) {}
bool check_lock_pass(const char *pass) { return false; }
esp_err_t httpd_resp_set_status(httpd_req_t *req, const char *status) { return 0; }
esp_err_t httpd_resp_set_hdr(httpd_req_t *req, const char *hdr, const char *val) { return 0; }
int httpd_req_recv(httpd_req_t *req, char *buf, size_t len) { return len; }
int crypto_memcmp(const void *a, const void *b, size_t len) { return 0; }
void update_lock_pass(const char *pass) {}
bool is_lock_pass_set() { return true; }
void httpd_resp_send_err(httpd_req_t *req, int err, const char *msg) {}

#define ESP_LOGI(...)
#define ESP_LOGE(...)
#define HTTPD_400_BAD_REQUEST 400
#define HTTPD_500_INTERNAL_SERVER_ERROR 500
#define HTTPD_SOCK_ERR_TIMEOUT -2
#define HTTP_GET 0
#define HTTP_POST 1
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ESP_FAIL -1
#define ESP_OK 0

const char ul_start[] = "start";
const char ul_end[] = "start_end";
const char l_start[] = "%s";
const char l_end[] = "start_end";
char *lock_pass = NULL;

bool isLocked();
void lockUI();

// Includes the pre-stripped source file
#include "lockhandler_stripped.c"

int main() {
    printf("Running test_lockhandler...\n");

    locked = false;
    assert(isLocked() == false);

    lockUI();
    assert(isLocked() == true);

    locked = false;
    assert(isLocked() == false);

    printf("All test_lockhandler passed!\n");
    return 0;
}
