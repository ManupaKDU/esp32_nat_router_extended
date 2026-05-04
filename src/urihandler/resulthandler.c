#include "handler.h"

#include <esp_wifi.h>

#include "nvs.h"
#include "cmd_nvs.h"
#include "router_globals.h"

static const char *TAG = "ResultHandler";

const char *ROW_TEMPLATE = "<tr><td class='text-%s'>%s</td><td class='text-%s'>%s</td><td><form action='/' method='POST'><input type='hidden' name='ssid' value='%s'><input type='submit' value='Use' name='use' class='btn btn-primary' aria-label='Use %s'/></form></td></tr>";

char *findTextColorForSSID(int8_t rssi)
{
    char *color;
    if (rssi >= -50)
    {
        color = "success";
    }
    else if (rssi >= -70)
    {
        color = "info";
    }
    else
    {
        color = "warning";
    }
    return color;
}

esp_err_t result_download_get_handler(httpd_req_t *req)
{
    if (isLocked())
    {
        return redirectToLock(req);
    }

    httpd_req_to_sockfd(req);

    extern const char result_start[] asm("_binary_result_html_start");
    extern const char result_end[] asm("_binary_result_html_end");
    const size_t result_html_size = (result_end - result_start);

    char *result_param = NULL;
    int allocatedSize = (strlen(ROW_TEMPLATE) + 100) * DEFAULT_SCAN_LIST_SIZE;

    char result[allocatedSize];
    strcpy(result, "");

    // Bolt Optimization: Replace O(N^2) strcat looping with a running offset
    // and snprintf, to make HTML string concatenation scale efficiently.
    size_t current_len = 0;

    get_config_param_str("scan_result", &result_param);
    if (result_param == NULL)
    {
        int added = snprintf(result, allocatedSize, "<tr><td colspan='3' class='text-muted'>No networks found</td></tr>");
        if (added > 0 && added < allocatedSize) {
            current_len += added;
        }
    }
    else
    {
        char *end_str;
        char *row = strtok_r(result_param, "\x05", &end_str);
        while (row != NULL)
        {
            char *ssid = strtok(row, "\x03");
            char *rssi = strtok(NULL, "\x03");

            if (ssid != NULL && rssi != NULL)
            {
                char sanitized_ssid[33 * 6];
                sanitize_html(ssid, sanitized_ssid, sizeof(sanitized_ssid));

                char *css = findTextColorForSSID(atoi(rssi));
                int added = snprintf(result + current_len, allocatedSize - current_len, ROW_TEMPLATE, css, sanitized_ssid, css, rssi, sanitized_ssid, sanitized_ssid);
                if (added > 0 && added < allocatedSize - current_len) {
                    current_len += added;
                }
            }

            row = strtok_r(NULL, "\x05", &end_str);
        }
    }

    int size = result_html_size + strlen(result);
    char *result_page = malloc(size + 1);
    // ⚡ Bolt: Cache response length from sprintf to avoid redundant O(N) strlen calls
    int response_len = sprintf(result_page, result_start, result);

    closeHeader(req);

    esp_err_t ret = httpd_resp_send(req, result_page, response_len);
    ESP_LOGI(TAG, "Requesting result page with  %d additional bytes", response_len);

    free(result_page);
    nvs_handle_t nvs;
    nvs_open(PARAM_NAMESPACE, NVS_READWRITE, &nvs);
    int32_t result_shown = 0;
    get_config_param_int("result_shown", &result_shown);
    nvs_set_i32(nvs, "result_shown", ++result_shown);
    ESP_LOGI(TAG, "Result shown %ld times", result_shown);

    nvs_commit(nvs);
    nvs_close(nvs);
    return ret;
}