#pragma once
typedef struct {
    const char *command;
    const char *help;
    const char *hint;
    int (*func)(int argc, char **argv);
    void *argtable;
} esp_console_cmd_t;
int esp_console_cmd_register(const esp_console_cmd_t *cmd);
#define ESP_ERROR_CHECK(x) (void)(x)
