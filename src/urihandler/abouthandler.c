#include "handler.h"

static const char *TAG = "AboutHandler";

esp_err_t about_get_handler(httpd_req_t *req)
{

    ESP_LOGI(TAG, "Requesting about page");
    if (isLocked())
    {
        return redirectToLock(req);
    }
    httpd_req_to_sockfd(req);

    extern const char about_start[] asm("_binary_about_html_start");
    extern const char about_end[] asm("_binary_about_html_end");
    const size_t about_html_size = (about_end - about_start);

    const char *project_version = get_project_version();
    const char *project_build_date = get_project_build_date();
    size_t size = about_html_size + strlen(project_version) + strlen(GLOBAL_HASH) + strlen(project_build_date) + 1;
    char *about_page = malloc(size);

    // ⚡ Bolt: Capture dynamic string length to avoid redundant O(N) strlen() in httpd_resp_send
    int response_len = snprintf(about_page, size, about_start, project_version, GLOBAL_HASH, project_build_date);

    closeHeader(req);

    esp_err_t out = httpd_resp_send(req, about_page, (response_len > 0 && response_len < size) ? response_len : HTTPD_RESP_USE_STRLEN);
    free(about_page);
    return out;
}