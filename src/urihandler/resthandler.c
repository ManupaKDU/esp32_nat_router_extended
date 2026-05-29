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
    int len = snprintf(json, sizeof(json), JSON_TEMPLATE, getConnectCount(), db, textColor);
    // ⚡ Bolt: Avoid O(N) strlen() call by passing the captured snprintf length, bound-checked against buffer size
    size_t resp_len = (len > 0 && len < sizeof(json)) ? (size_t)len : sizeof(json) - 1;
    esp_err_t ret = httpd_resp_send(req, json, resp_len);
    ESP_LOGD(TAG, "JSON-Response: %s", json);
    return ret;
}