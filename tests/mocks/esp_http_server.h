#ifndef ESP_HTTP_SERVER_H
#define ESP_HTTP_SERVER_H

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

typedef struct {
    int content_len;
} httpd_req_t;

#define HTTPD_SOCK_ERR_TIMEOUT -1

int httpd_query_key_value(const char *qry, const char *key, char *val, size_t val_size);
int httpd_req_recv(httpd_req_t *r, char *buf, size_t buf_len);

#endif
