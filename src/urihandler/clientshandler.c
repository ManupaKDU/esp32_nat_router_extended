#include "handler.h"
#include "timer.h"

#include <sys/param.h>
#include <timer.h>

#include "nvs.h"
#include "cmd_nvs.h"
#include "router_globals.h"
#include "esp_wifi.h"
#include "esp_wifi_ap_get_sta_list.h"

static const char *TAG = "ClientsHandler";

const char *CLIENT_TEMPLATE = "<tr><td>%i</td><td>%s</td><td style='text-transform: uppercase;'>%s</td></tr>";

esp_err_t clients_download_get_handler(httpd_req_t *req)
{
    if (isLocked())
    {
        return redirectToLock(req);
    }

    char result[1000];
    int offset = 0;
    result[0] = '\0';

    // ⚡ Bolt: Check cached connect count before performing expensive Wi-Fi list queries
    if (getConnectCount() > 0)
    {
        wifi_sta_list_t wifi_sta_list;
        wifi_sta_mac_ip_list_t adapter_sta_list;
        memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
        memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
        esp_wifi_ap_get_sta_list(&wifi_sta_list);

        esp_wifi_ap_get_sta_list_with_ip(&wifi_sta_list, &adapter_sta_list);

        for (int i = 0; i < adapter_sta_list.num; i++)
        {
            esp_netif_pair_mac_ip_t station = adapter_sta_list.sta[i];

            char str_ip[16];
            esp_ip4addr_ntoa(&(station.ip), str_ip, IP4ADDR_STRLEN_MAX);

            char currentMAC[18];
            snprintf(currentMAC, sizeof(currentMAC), "%x:%x:%x:%x:%x:%x", station.mac[0], station.mac[1], station.mac[2], station.mac[3], station.mac[4], station.mac[5]);

            // ⚡ Bolt: Prevent O(N^2) string concatenation by writing directly to buffer at the current offset
            int written = snprintf(result + offset, sizeof(result) - offset, CLIENT_TEMPLATE, i + 1, str_ip, currentMAC);
            if (written > 0 && written < sizeof(result) - offset) {
                offset += written;
            } else {
                offset += strlen(result + offset); // Update offset accurately on truncation
                break; // Prevent buffer overflow if we exceed result buffer size
            }
        }
    }
    else
    {
        const char *no_clients = "<tr class='text-muted'><td colspan='3'>No clients connected</td></tr>";
        strncpy(result, no_clients, sizeof(result) - 1);
        result[sizeof(result) - 1] = '\0';
        offset = strlen(result);
    }

    httpd_req_to_sockfd(req);
    extern const char clients_start[] asm("_binary_clients_html_start");
    extern const char clients_end[] asm("_binary_clients_html_end");
    const size_t clients_html_size = (clients_end - clients_start);

    // ⚡ Bolt: Reused offset instead of calling strlen(result) again
    int size = clients_html_size + offset;
    char *clients_page = malloc(size - 2);
    int clients_page_len = snprintf(clients_page, size - 2, clients_start, result);

    closeHeader(req);

    // ⚡ Bolt: Use captured length from snprintf instead of HTTPD_RESP_USE_STRLEN (which does strlen internally)
    esp_err_t ret = httpd_resp_send(req, clients_page, clients_page_len);
    free(clients_page);
    ESP_LOGI(TAG, "Requesting clients page");
    return ret;
}
