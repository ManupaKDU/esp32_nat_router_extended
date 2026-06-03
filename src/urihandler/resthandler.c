#include "handler.h"
#include "router_globals.h"

static const char *TAG = "RestHandler";

const char *JSON_TEMPLATE = "{\"clients\": %d,\"strength\": %s,\"text\": \"%s\"}";

esp_err_t rest_handler(httpd_req_t *req)
{
    if (isLocked())
    {
        return httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    }
    httpd_resp_set_type(req, "application/json");

    char db[8] = "";
    char *textColor = NULL;
    fillInfoData(db, &textColor);

    size_t size = strlen(JSON_TEMPLATE) + 5 + strlen(db) + strlen(textColor);
    char json[size]; // ⚡ Bolt: Use stack VLA instead of malloc to avoid heap fragmentation overhead

    // ⚡ Bolt: Capture explicit string length from snprintf to avoid O(N) strlen() in httpd_resp_send
    int response_len = snprintf(json, sizeof(json), JSON_TEMPLATE, getConnectCount(), db, textColor);
    // Ensure response_len doesn't exceed the actual buffer size in case of truncation
    if (response_len > sizeof(json) - 1) {
        response_len = sizeof(json) - 1;
    }
    esp_err_t ret = httpd_resp_send(req, json, response_len);

    ESP_LOGD(TAG, "JSON-Response: %s", json);
    return ret;
}