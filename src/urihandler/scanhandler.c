
#include "handler.h"
#include "scan.h"

#include "router_globals.h"

static const char *TAG = "ScanHandler";

void fillInfoData(char *db, char **textColor)
{
    // ⚡ Bolt: Removed realloc and using stack-allocated buffer to prevent memory fragmentation
    wifi_ap_record_t apinfo;
    memset(&apinfo, 0, sizeof(apinfo));
    if (esp_wifi_sta_get_ap_info(&apinfo) == ESP_OK)
    {
        sprintf(db, "%d", apinfo.rssi);
        *textColor = findTextColorForSSID(apinfo.rssi);
        ESP_LOGD(TAG, "RSSI: %d", apinfo.rssi);
        ESP_LOGD(TAG, "SSID: %s", apinfo.ssid);
    }
    else
    {
        sprintf(db, "%d", 0);
        *textColor = "muted";
    }
}

esp_err_t scan_download_get_handler(httpd_req_t *req)
{

    if (isLocked())
    {
        return redirectToLock(req);
    }

    httpd_req_to_sockfd(req);

    char *defaultIP = getDefaultIPByNetmask();

    extern const char scan_start[] asm("_binary_scan_html_start");
    extern const char scan_end[] asm("_binary_scan_html_end");
    const size_t scan_html_size = (scan_end - scan_start);

    size_t alloc_size = scan_html_size + strlen(defaultIP);
    char *scan_page = malloc(alloc_size);

    // ⚡ Bolt: Capture dynamic string length to avoid redundant O(N) strlen() in httpd_resp_send
    int response_len = snprintf(scan_page, alloc_size, scan_start, defaultIP);

    closeHeader(req);

    ESP_LOGI(TAG, "Requesting scan page");

    esp_err_t ret = httpd_resp_send(req, scan_page, (response_len > 0 && response_len < alloc_size) ? response_len : HTTPD_RESP_USE_STRLEN);
    fillNodes();
    free(scan_page);
    free(defaultIP);
    return ret;
}
