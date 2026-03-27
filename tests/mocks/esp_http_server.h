#ifndef ESP_HTTP_SERVER_H
#define ESP_HTTP_SERVER_H

#include <stddef.h>

typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1

typedef struct {
    size_t content_len;
} httpd_req_t;

#define HTTPD_SOCK_ERR_TIMEOUT -1

int httpd_query_key_value(char *parameterString, char *parameter, char *buffer, size_t paramLength);
int httpd_req_recv(httpd_req_t *r, char *buf, size_t len);

#endif
