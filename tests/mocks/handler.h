#ifndef HANDLER_H
#define HANDLER_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Mock definitions
typedef int esp_err_t;
typedef struct httpd_req httpd_req_t;

// Function declarations we want to test
char *findTextColorForSSID(int8_t rssi);

#endif
