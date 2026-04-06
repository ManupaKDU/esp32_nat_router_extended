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
    snprintf(json, sizeof(json), JSON_TEMPLATE, getConnectCount(), db, textColor);
    esp_err_t ret = httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
    ESP_LOGD(TAG, "JSON-Response: %s", json);
    return ret;
}