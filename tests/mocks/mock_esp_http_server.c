#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "esp_http_server.h"

// Mock for httpd_query_key_value
// For simplicity we just set the val to a preset or leave it empty, but a real parsing mock could be done if needed.
// This simple mock will just look for "testparam=value" string
int httpd_query_key_value(const char *qry, const char *key, char *val, size_t val_size) {
    char search_key[100];
    snprintf(search_key, sizeof(search_key), "%s=", key);

    char *p = strstr(qry, search_key);
    if (p) {
        p += strlen(search_key);
        char *q = strchr(p, '&');
        size_t len = q ? (size_t)(q - p) : strlen(p);
        if (len >= val_size) len = val_size - 1;
        strncpy(val, p, len);
        val[len] = '\0';
        return ESP_OK;
    }
    return ESP_FAIL;
}

// Mock for httpd_req_recv
static char *mock_recv_data = NULL;
static size_t mock_recv_len = 0;
static size_t mock_recv_offset = 0;
static int mock_recv_ret_val = 0;
static bool mock_timeout = false;

void set_mock_req_recv_data(char *data, size_t len, int ret_val, bool timeout) {
    mock_recv_data = data;
    mock_recv_len = len;
    mock_recv_offset = 0;
    mock_recv_ret_val = ret_val;
    mock_timeout = timeout;
}

int httpd_req_recv(httpd_req_t *r, char *buf, size_t buf_len) {
    if (mock_timeout) {
        mock_timeout = false; // Only timeout once for test
        return HTTPD_SOCK_ERR_TIMEOUT;
    }

    if (mock_recv_ret_val < 0) {
        int ret = mock_recv_ret_val;
        mock_recv_ret_val = 0; // Prevent infinite loop in fill_post_buffer
        return ret;
    }

    size_t to_copy = mock_recv_len - mock_recv_offset;
    if (to_copy == 0) return 0; // EOF

    if (to_copy > buf_len) to_copy = buf_len;

    if (to_copy > 0) {
        if (mock_recv_data) {
            memcpy(buf, mock_recv_data + mock_recv_offset, to_copy);
        }
        mock_recv_offset += to_copy;
    }
    return to_copy;
}
