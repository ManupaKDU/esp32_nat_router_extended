#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

// Mock esp-idf functions that cmd_nvs.c relies on, allowing us to include it
#define FREERTOS_FREERTOS_H
#define FREERTOS_EVENT_GROUPS_H

#include "nvs.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_err.h"

// Dummy NVS functions
esp_err_t nvs_open(const char* name, int open_mode, nvs_handle_t *out_handle) { return ESP_OK; }
esp_err_t nvs_set_i8 (nvs_handle_t handle, const char* key, int8_t value) { return ESP_OK; }
esp_err_t nvs_set_u8 (nvs_handle_t handle, const char* key, uint8_t value) { return ESP_OK; }
esp_err_t nvs_set_i16 (nvs_handle_t handle, const char* key, int16_t value) { return ESP_OK; }
esp_err_t nvs_set_u16 (nvs_handle_t handle, const char* key, uint16_t value) { return ESP_OK; }
esp_err_t nvs_set_i32 (nvs_handle_t handle, const char* key, int32_t value) { return ESP_OK; }
esp_err_t nvs_set_u32 (nvs_handle_t handle, const char* key, uint32_t value) { return ESP_OK; }
esp_err_t nvs_set_i64 (nvs_handle_t handle, const char* key, int64_t value) { return ESP_OK; }
esp_err_t nvs_set_u64 (nvs_handle_t handle, const char* key, uint64_t value) { return ESP_OK; }
esp_err_t nvs_set_str (nvs_handle_t handle, const char* key, const char* value) { return ESP_OK; }
esp_err_t nvs_set_blob(nvs_handle_t handle, const char* key, const void* value, size_t length) { return ESP_OK; }
esp_err_t nvs_commit(nvs_handle_t handle) { return ESP_OK; }
void nvs_close(nvs_handle_t handle) {}
esp_err_t nvs_get_i8 (nvs_handle_t handle, const char* key, int8_t* out_value) { return ESP_OK; }
esp_err_t nvs_get_u8 (nvs_handle_t handle, const char* key, uint8_t* out_value) { return ESP_OK; }
esp_err_t nvs_get_i16 (nvs_handle_t handle, const char* key, int16_t* out_value) { return ESP_OK; }
esp_err_t nvs_get_u16 (nvs_handle_t handle, const char* key, uint16_t* out_value) { return ESP_OK; }
esp_err_t nvs_get_i32 (nvs_handle_t handle, const char* key, int32_t* out_value) { return ESP_OK; }
esp_err_t nvs_get_u32 (nvs_handle_t handle, const char* key, uint32_t* out_value) { return ESP_OK; }
esp_err_t nvs_get_i64 (nvs_handle_t handle, const char* key, int64_t* out_value) { return ESP_OK; }
esp_err_t nvs_get_u64 (nvs_handle_t handle, const char* key, uint64_t* out_value) { return ESP_OK; }
esp_err_t nvs_get_str (nvs_handle_t handle, const char* key, char* out_value, size_t* length) { return ESP_OK; }
esp_err_t nvs_get_blob(nvs_handle_t handle, const char* key, void* out_value, size_t* length) { return ESP_OK; }
esp_err_t nvs_erase_key(nvs_handle_t handle, const char* key) { return ESP_OK; }
esp_err_t nvs_erase_all(nvs_handle_t handle) { return ESP_OK; }

typedef struct {
    char namespace_name[16];
    char key[16];
    nvs_type_t type;
} nvs_entry_info_t;

typedef void* nvs_iterator_t;

esp_err_t nvs_entry_find(const char *part_name, const char *namespace_name, nvs_type_t type, nvs_iterator_t *output_iterator) { return ESP_OK; }
esp_err_t nvs_entry_info(nvs_iterator_t it, nvs_entry_info_t *out_info) { return ESP_OK; }
esp_err_t nvs_entry_next(nvs_iterator_t *it) { return ESP_OK; }
void nvs_release_iterator(nvs_iterator_t it) {}

#define esp_err_to_name(x) "ESP_OK"

struct arg_str* arg_str1(const char *shortopts, const char *longopts, const char *datatype, const char *glossary) { return NULL; }
struct arg_str* arg_str0(const char *shortopts, const char *longopts, const char *datatype, const char *glossary) { return NULL; }
struct arg_end* arg_end(int maxcount) { return NULL; }
int arg_parse(int argc, char **argv, void **argtable) { return 0; }
void arg_print_errors(void *fp, struct arg_end *end, const char *progname) {}
int esp_console_cmd_register(const esp_console_cmd_t *cmd) { return ESP_OK; }

// Include the source file so we can test its static functions directly
#include "../components/cmd_nvs/cmd_nvs.c"

void test_type_to_str() {
    printf("Running test_type_to_str...\n");
    assert(strcmp(type_to_str(NVS_TYPE_I8), "i8") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_U8), "u8") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_U16), "u16") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_I16), "i16") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_U32), "u32") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_I32), "i32") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_U64), "u64") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_I64), "i64") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_STR), "str") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_BLOB), "blob") == 0);
    assert(strcmp(type_to_str(NVS_TYPE_ANY), "any") == 0);

    // Invalid/Unknown type
    assert(strcmp(type_to_str((nvs_type_t)999), "Unknown") == 0);

    printf("All test_type_to_str passed!\n");
}

int main() {
    test_type_to_str();
    return 0;
}
