#ifndef HELPER_H
#define HELPER_H

#include "esp_log.h"
#include "esp_http_server.h"
#include "sys/param.h"
#include "lwip/inet.h"
#include <stdint.h>
#include <stdbool.h>

void preprocess_string(char *str);
void readUrlParameterIntoBuffer(char *parameterString, char *parameter, char *buffer, size_t paramLength);
esp_err_t fill_post_buffer(httpd_req_t *req, char *buf, size_t len);
bool is_valid_subnet_mask(char *subnet_mask);
bool str2mac(const char *mac, uint8_t *values);
int crypto_memcmp(const void *a, const void *b, size_t len);
void sanitize_html(const char *input, char *output, size_t output_size);

#endif
