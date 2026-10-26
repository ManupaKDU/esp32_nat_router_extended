#include "handler.h"
#include <sys/param.h>
#include "dhcpserver/dhcpserver.h"
#include "router_globals.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_err.h"

static const char *TAG = "Advancedhandler";

esp_err_t advanced_download_get_handler(httpd_req_t *req)
{
    if (isLocked())
    {
        return redirectToLock(req);
    }

    httpd_req_to_sockfd(req);
    extern const char advanced_start[] asm("_binary_advanced_html_start");
    extern const char advanced_end[] asm("_binary_advanced_html_end");
    const size_t advanced_html_size = (advanced_end - advanced_start);

    int32_t keepAlive = 0;
    int32_t ledDisabled = 0;
    int32_t natDisabled = 0;
    char *aliveCB = "";
    char *ledCB = "";
    char *natCB = "";
    char *allocatedDNS = NULL;
    char *currentDNS = "";
    char *defCB = "";
    char *cloudCB = "";
    char *adguardCB = "";
    char *customCB = "";
    char *customDNSIP = "";
    char *defMacCB = "";
    char *rndMacCB = "";
    char *customMacCB = "";
    char *customMac = "";
    char *macSetting = NULL;
    char *classACB = "";
    char *classBCB = "";
    char *classCCB = "";
    char *customMaskCB = "";
    char *customMask = "";

    char currentMAC[18];
    char defaultMAC[18];

    // ⚡ Bolt: Batch NVS fetches to eliminate repeated nvs_open/nvs_close overhead
    nvs_handle_t param_nvs;
    esp_err_t nvs_status = nvs_open(PARAM_NAMESPACE, NVS_READONLY, &param_nvs);

    char *hostName = NULL;
    int32_t octet = 4;
    if (nvs_status == ESP_OK) get_config_param_str_from_nvs(param_nvs, "hostname", &hostName);
    if (nvs_status == ESP_OK) get_config_param_int_from_nvs(param_nvs, "octet", &octet);

    int32_t txPower = 0;
    if (nvs_status == ESP_OK) get_config_param_int_from_nvs(param_nvs, "txpower", &txPower);
    if (txPower < 8 || txPower > 84)
    {
        txPower = 80; // default
    }
    char *lowSelected, *mediumSelected, *highSelected = NULL;
    if (txPower < 34)
    { // low
        lowSelected = "selected";
        mediumSelected = "";
        highSelected = "";
    }
    else if (txPower < 60)
    { // medium
        lowSelected = "";
        mediumSelected = "selected";
        highSelected = "";
    }
    else
    { // high
        lowSelected = "";
        mediumSelected = "";
        highSelected = "selected";
    }

    int32_t bandwith = 0;
    if (nvs_status == ESP_OK) get_config_param_int_from_nvs(param_nvs, "lower_bandwith", &bandwith);
    char *bwLow, *bwHigh = NULL;
    if (bandwith == 1)
    {
        bwLow = "selected";
        bwHigh = "";
    }
    else
    {
        bwLow = "";
        bwHigh = "selected";
    }

    if (nvs_status == ESP_OK) get_config_param_int_from_nvs(param_nvs, "keep_alive", &keepAlive);
    if (keepAlive == 1)
    {
        aliveCB = "checked";
    }
    if (nvs_status == ESP_OK) get_config_param_int_from_nvs(param_nvs, "led_disabled", &ledDisabled);
    if (ledDisabled == 0)
    {
        ledCB = "checked";
    }
    if (nvs_status == ESP_OK) get_config_param_int_from_nvs(param_nvs, "nat_disabled", &natDisabled);
    if (natDisabled == 0)
    {
        natCB = "checked";
    }
    int32_t bridgeEnabled = 0;
    char *bridgeCB = "";
    if (nvs_status == ESP_OK) get_config_param_int_from_nvs(param_nvs, "bridge_enabled", &bridgeEnabled);
    if (bridgeEnabled == 1)
    {
        bridgeCB = "checked";
    }
    esp_netif_dns_info_t dns;
    esp_netif_t *wifiSTA = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (esp_netif_get_dns_info(wifiSTA, ESP_NETIF_DNS_MAIN, &dns) == ESP_OK)
    {
        allocatedDNS = malloc(16);
        currentDNS = allocatedDNS;
        snprintf(currentDNS, 16, IPSTR, IP2STR(&(dns.ip.u_addr.ip4)));
        ESP_LOGI(TAG, "Current DNS is: %s", currentDNS);
    }

    char *customDNS = NULL;
    if (nvs_status == ESP_OK) get_config_param_str_from_nvs(param_nvs, "custom_dns", &customDNS);

    if (customDNS == NULL)
    {
        defCB = "checked";
    }
    else if (strcmp(customDNS, "1.1.1.1") == 0)
    {
        cloudCB = "checked";
    }
    else if (strcmp(customDNS, "94.140.14.14") == 0)
    {
        adguardCB = "checked";
    }
    else
    {
        customCB = "checked";
        // ⚡ Bolt: Reuse customDNS instead of redundant NVS read and malloc
        customDNSIP = customDNS;
    }

    uint8_t base_mac_addr[6] = {0};
    uint8_t default_mac_addr[6] = {0};
    ESP_ERROR_CHECK(esp_base_mac_addr_get(base_mac_addr));
    ESP_ERROR_CHECK(esp_efuse_mac_get_default(default_mac_addr));

    snprintf(currentMAC, sizeof(currentMAC), "%02x:%02x:%02x:%02x:%02x:%02x", base_mac_addr[0], base_mac_addr[1], base_mac_addr[2], base_mac_addr[3], base_mac_addr[4], base_mac_addr[5]);
    snprintf(defaultMAC, sizeof(defaultMAC), "%02x:%02x:%02x:%02x:%02x:%02x", default_mac_addr[0], default_mac_addr[1], default_mac_addr[2], default_mac_addr[3], default_mac_addr[4], default_mac_addr[5]);

    if (nvs_status == ESP_OK) get_config_param_str_from_nvs(param_nvs, "custom_mac", &macSetting);

    if (macSetting != NULL && strcmp(macSetting, "random") == 0)
    {
        rndMacCB = "checked";
    }

    else if (strcmp(currentMAC, defaultMAC) == 0)
    {
        defMacCB = "checked";
    }
    else
    {
        customMacCB = "checked";
        customMac = currentMAC;
    }

    char *netmask_alloc = NULL;
    if (nvs_status == ESP_OK) get_config_param_str_from_nvs(param_nvs, "netmask", &netmask_alloc);

    if (nvs_status == ESP_OK)
    {
        nvs_close(param_nvs);
    }

    char *netmask = netmask_alloc != NULL ? netmask_alloc : DEFAULT_NETMASK_CLASS_C;

    if (strcmp(netmask, DEFAULT_NETMASK_CLASS_A) == 0)
    {
        classACB = "checked";
    }

    else if (strcmp(netmask, DEFAULT_NETMASK_CLASS_B) == 0)
    {
        classBCB = "checked";
    }
    else if (strcmp(netmask, DEFAULT_NETMASK_CLASS_C) == 0)
    {
        classCCB = "checked";
    }
    else
    {
        customMaskCB = "checked";
        customMask = netmask;
    }

    char *safe_hostName = NULL;
    if (hostName) {
        size_t h_len = strlen(hostName) * 6 + 1;
        safe_hostName = malloc(h_len);
        if (safe_hostName) sanitize_html(hostName, safe_hostName, h_len);
    }

    char *safe_customDNSIP = NULL;
    if (customDNSIP && customDNSIP[0] != '\0') {
        size_t d_len = strlen(customDNSIP) * 6 + 1;
        safe_customDNSIP = malloc(d_len);
        if (safe_customDNSIP) sanitize_html(customDNSIP, safe_customDNSIP, d_len);
    }

    char *safe_customMac = NULL;
    if (customMac && customMac[0] != '\0') {
        size_t m_len = strlen(customMac) * 6 + 1;
        safe_customMac = malloc(m_len);
        if (safe_customMac) sanitize_html(customMac, safe_customMac, m_len);
    }

    char *safe_netmask = NULL;
    if (netmask && netmask[0] != '\0') {
        size_t nm_len = strlen(netmask) * 6 + 1;
        safe_netmask = malloc(nm_len);
        if (safe_netmask) sanitize_html(netmask, safe_netmask, nm_len);
    }

    char *safe_customMask = NULL;
    if (customMask && customMask[0] != '\0') {
        size_t cm_len = strlen(customMask) * 6 + 1;
        safe_customMask = malloc(cm_len);
        if (safe_customMask) sanitize_html(customMask, safe_customMask, cm_len);
    }

    int defaultMAC_len = strlen(defaultMAC);
    u_int size = advanced_html_size +
                 (keepAlive == 1 ? sizeof("checked") - 1 : 0) +
                 (ledDisabled == 0 ? sizeof("checked") - 1 : 0) +
                 (natDisabled == 0 ? sizeof("checked") - 1 : 0) +
                 (bridgeEnabled == 1 ? sizeof("checked") - 1 : 0) +
                 strlen(currentDNS) +
                 strlen(currentMAC) +
                 3 * (sizeof("checked") - 1) +
                 (safe_customDNSIP ? strlen(safe_customDNSIP) : 0) +
                 2 * defaultMAC_len +
                 (safe_customMac ? strlen(safe_customMac) : 0) +
                 (safe_netmask ? strlen(safe_netmask) : 0) +
                 (safe_hostName ? strlen(safe_hostName) : 0) +
                 2 * (sizeof("selected") - 1) +
                 (safe_customMask ? strlen(safe_customMask) : 0) +
                 4 /* 4 * Octet - 4 *%d*/ +
                 256;
    ESP_LOGI(TAG, "Allocating additional %d bytes for advanced page.", size);
    char *advanced_page = malloc(size);
    if (advanced_page == NULL)
    {
        ESP_LOGE(TAG, "Memory allocation failed for advanced page");
        free(allocatedDNS);
        free(hostName);
        free(customDNS);
        free(macSetting);
        free(netmask_alloc);
        if (safe_hostName) free(safe_hostName);
        if (safe_customDNSIP) free(safe_customDNSIP);
        if (safe_customMac) free(safe_customMac);
        if (safe_netmask) free(safe_netmask);
        if (safe_customMask) free(safe_customMask);
        return ESP_FAIL;
    }

    char subMac[18]; // ⚡ Bolt: Use stack buffer to avoid malloc overhead for small string
    strncpy(subMac, defaultMAC, sizeof(subMac) - 1);
    subMac[sizeof(subMac) - 1] = '\0';
    if (strlen(subMac) >= 2)
    {
        subMac[strlen(subMac) - 2] = '\0';
    }

    // ⚡ Bolt: Capture dynamic string length to avoid redundant O(N) strlen() in httpd_resp_send
    // 🛡️ Sentinel: Used sanitized configuration strings to prevent XSS injection when rendering template
    int response_len = snprintf(advanced_page, size, advanced_start,
        safe_hostName ? safe_hostName : "", octet, lowSelected, mediumSelected, highSelected,
        bwHigh, bwLow, ledCB, aliveCB, natCB, bridgeCB, currentDNS, defCB, cloudCB,
        adguardCB, customCB, safe_customDNSIP ? safe_customDNSIP : "", currentMAC, defMacCB,
        defaultMAC, rndMacCB, subMac, customMacCB, safe_customMac ? safe_customMac : "",
        safe_netmask ? safe_netmask : "", classCCB, octet, classBCB, octet, classACB, octet,
        customMaskCB, safe_customMask ? safe_customMask : "");

    closeHeader(req);
    esp_err_t ret = httpd_resp_send(req, advanced_page, (response_len > 0 && response_len < size) ? response_len : HTTPD_RESP_USE_STRLEN);

    free(advanced_page);
    if (safe_hostName) free(safe_hostName);
    if (safe_customDNSIP) free(safe_customDNSIP);
    if (safe_customMac) free(safe_customMac);
    if (safe_netmask) free(safe_netmask);
    if (safe_customMask) free(safe_customMask);
    free(allocatedDNS);
    free(hostName);
    free(customDNS);
    if (macSetting != NULL)
    {
        free(macSetting);
    }
    free(netmask_alloc);

    return ret;
}
