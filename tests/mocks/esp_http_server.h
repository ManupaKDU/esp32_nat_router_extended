#ifndef ESP_HTTP_SERVER_H
#define ESP_HTTP_SERVER_H
#include <stddef.h>
typedef struct { int content_len; } httpd_req_t;
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define HTTPD_SOCK_ERR_TIMEOUT -2
int httpd_query_key_value(const char *qry, const char *key, char *val, size_t val_size);
int httpd_req_recv(httpd_req_t *r, char *buf, size_t buf_len);
#endif
