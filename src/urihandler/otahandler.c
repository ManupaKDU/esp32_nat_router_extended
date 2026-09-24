
#include "handler.h"
#include <esp_ota_ops.h>
#include <esp_https_ota.h>
#include <esp_log.h>
#include <sys/param.h>
#include "timer.h"
#include <esp_http_client.h>

static const char *TAG = "OTA";

static const char *NOT_DETERMINED = "Not determined yet";
static const char *ERROR_RETRIEVING = "Error retrieving the data. HTTP-Code: %d";
static char latest_version[50] = "";
static char changelog[400] = "";
static size_t changelog_len = 0;
bool finished = false;
bool otaRunning = false;
static bool otaSuccess = false;

char chip_type[30];

char otalog[400] = "";
static size_t otalog_len = 0;
char resultLog[110] = "";
char progressLabel[20] = "";

typedef struct
{
    int http_code;
} http_handler_data_t;

static const char *DEFAULT_URL = "https://raw.githubusercontent.com/ManupaKDU/esp32_nat_router_extended/releases-production/";
static const char *DEFAULT_URL_CANARY = "https://raw.githubusercontent.com/ManupaKDU/esp32_nat_router_extended/releases-staging/";

void appendToLog(const char *message)
{
    if (otalog_len < sizeof(otalog) - 1)
    {
        int added = snprintf(otalog + otalog_len, sizeof(otalog) - otalog_len, "<tr><td>%s</td></tr>", message);
        if (added > 0 && added < sizeof(otalog) - otalog_len)
        {
            otalog_len += added;
        }
        else if (added >= sizeof(otalog) - otalog_len)
        {
            otalog_len = sizeof(otalog) - 1;
        }
    }
    ESP_LOGI(TAG, "%s", message);
}

void setResultLog(const char *message, const char *cssClass)
{

    snprintf(resultLog, sizeof(resultLog), "<tr><td class=\"%s\">%s</td></tr>", cssClass, message);

    ESP_LOGI(TAG, "%s", message);
}

#define DOWNLOAD_TIMEOUT_MS 30000
char *file_buffer = NULL;
size_t file_size = 0;

int64_t data_length = 0;
int64_t content_length = 0;
int threshold = 0;
int progressInt = 0;

// HTTP-Client-Event-Handler
esp_err_t ota_event_event_handler(esp_http_client_event_t *evt)
{
    char tmp[50] = "";

    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:

        data_length = data_length + ((int64_t)evt->data_len * 1000);
        if (content_length != 0)
        {
            int64_t progress = (data_length * 100) / content_length / 1000;
            progressInt = (int)progress;
            snprintf(progressLabel, sizeof(progressLabel), "%d of %d kB", (int)(data_length / 1000 / 1000), (int)(content_length / 1000));

            if (progressInt >= threshold) // do not flood log
            {
                threshold = threshold + 10;
                ESP_LOGI(TAG, "%s", progressLabel);
            }
        }

        break;
    case HTTP_EVENT_ERROR:
        return ESP_FAIL;
    case HTTP_EVENT_ON_HEADER:

        char *endptr;
        if (strcasecmp("Content-Length", evt->header_key) == 0)
        {
            content_length = strtol(evt->header_value, &endptr, 10);
            snprintf(tmp, sizeof(tmp), "Download size is %lld kB", (content_length / 1000));
            appendToLog(tmp);
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t version_event_handler(esp_http_client_event_t *evt)
{

    http_handler_data_t *handler_data = (http_handler_data_t *)evt->user_data;

    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            size_t new_size = file_size + evt->data_len;
            char *new_buf = realloc(file_buffer, new_size + 1);
            if (new_buf != NULL)
            {
                file_buffer = new_buf;
                memcpy(file_buffer + file_size, evt->data, evt->data_len);
                file_size = new_size;
                file_buffer[file_size] = '\0';
            }
            else
            {
                ESP_LOGE(TAG, "Realloc failed in version_event_handler");
            }
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        int http_status_code = esp_http_client_get_status_code(evt->client);
        handler_data->http_code = http_status_code;
        ESP_LOGI(TAG, "Download finished");
        break;
    default:
        break;
    }
    return ESP_OK;
}

const char *get_default_url()
{
    int32_t canary = 0;
    get_config_param_int("canary", &canary);
    if (canary == 1)
    {
        return DEFAULT_URL_CANARY;
    }

    return DEFAULT_URL;
}

void getOtaUrl(char *url, size_t url_size, char *label, size_t label_size)
{
    char *customUrl = NULL;
    get_config_param_str("ota_url", &customUrl);
    if (customUrl != NULL && customUrl[0] != '\0')
    {
        ESP_LOGI(TAG, "Custom Url found '%s'", customUrl);
        snprintf(label, label_size, "Custom build");
        size_t len = strlen(customUrl);
        if (len >= 4 && strcmp(customUrl + len - 4, ".bin") == 0)
        {
            snprintf(url, url_size, "%s", customUrl);
        }
        else if (customUrl[len - 1] == '/')
        {
            snprintf(url, url_size, "%s%s/firmware.bin", customUrl, chip_type);
        }
        else
        {
            snprintf(url, url_size, "%s/%s/firmware.bin", customUrl, chip_type);
        }
    }
    else
    {
        const char *usedUrl = get_default_url();
        if (strcmp(usedUrl, DEFAULT_URL_CANARY) == 0)
        {
            snprintf(label, label_size, "Canary build");
        }
        else
        {
            snprintf(label, label_size, "Default build");
        }

        snprintf(url, url_size, "%s%s/firmware.bin", usedUrl, chip_type);
    }
    if (customUrl != NULL)
    {
        free(customUrl);
    }
}

void ota_task(void *pvParameter)
{
    data_length = 0;
    content_length = 0;
    threshold = 0;
    char url[200];
    char label[20];

    getOtaUrl(url, sizeof(url), label, sizeof(label));

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = ota_event_event_handler,
        .skip_cert_common_name_check = true,
        .timeout_ms = DOWNLOAD_TIMEOUT_MS};

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    finished = false;
    otaSuccess = false;
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK)
    {
        otaSuccess = true;
        setResultLog("OTA update successful. The device is restarting.", "text-success");
    }
    else
    {
        otaSuccess = false;
        setResultLog("OTA update failed. Check network/URL and try again.", "text-danger");
    }
    finished = true;
    vTaskDelete(NULL);
}

void start_ota_update()
{
    xTaskCreate(&ota_task, "ota_task", 8192, NULL, 5, NULL);
}

void appendToChangelog(const char *entry)
{
    if (changelog_len < sizeof(changelog) - 1)
    {
        int added = snprintf(changelog + changelog_len, sizeof(changelog) - changelog_len, "<li>%s</li>", entry);
        if (added > 0 && added < sizeof(changelog) - changelog_len)
        {
            changelog_len += added;
        }
        else if (added >= sizeof(changelog) - changelog_len)
        {
            changelog_len = sizeof(changelog) - 1;
        }
    }
}

void updateVersion()
{
    char *customUrl = NULL;
    get_config_param_str("ota_url", &customUrl);
    char url[256];
    if (customUrl != NULL && customUrl[0] != '\0')
    {
        size_t len = strlen(customUrl);
        if (len >= 4 && strcmp(customUrl + len - 4, ".bin") == 0)
        {
            char *last_slash = strrchr(customUrl, '/');
            if (last_slash != NULL)
            {
                int base_len = last_slash - customUrl + 1;
                snprintf(url, sizeof(url), "%.*sversion", base_len, customUrl);
            }
            else
            {
                snprintf(url, sizeof(url), "%s", customUrl);
            }
        }
        else if (customUrl[len - 1] == '/')
        {
            snprintf(url, sizeof(url), "%sversion", customUrl);
        }
        else
        {
            snprintf(url, sizeof(url), "%s/version", customUrl);
        }
    }
    else
    {
        const char *usedUrl = get_default_url();
        snprintf(url, sizeof(url), "%sversion", usedUrl);
    }
    if (customUrl != NULL)
    {
        free(customUrl);
    }

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = version_event_handler,
        .user_data = &(http_handler_data_t){.http_code = 200},
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_timeout_ms(client, DOWNLOAD_TIMEOUT_MS);
    esp_err_t err = esp_http_client_perform(client);
    changelog[0] = '\0';
    changelog_len = 0;
    http_handler_data_t *handler_data = (http_handler_data_t *)config.user_data;

    if (err == ESP_OK && handler_data->http_code == 200 && file_buffer != NULL && file_size > 0)
    {
        ESP_LOGI(TAG, "Version and changelog download successful. File size is: %d Bytes", (int)file_size);
        file_buffer[file_size] = '\0';
        char *rest = file_buffer;
        char *line;
        int lineNumber = 1;
        while ((line = strtok_r(rest, "\n", &rest)) != NULL)
        {
            ESP_LOGD(TAG, "Line %d: %s", lineNumber, line);
            switch (lineNumber)
            {
            case 1:
                strncpy(latest_version, line, sizeof(latest_version) - 1);
                latest_version[sizeof(latest_version) - 1] = '\0';
                break;

            default:
                appendToChangelog(line);
                break;
            }
            lineNumber++;
        }
    }
    else
    {
        ESP_LOGE(TAG, "Error on download: %s -> %d", esp_err_to_name(err), handler_data->http_code);
        snprintf(latest_version, sizeof(latest_version), ERROR_RETRIEVING, handler_data->http_code);
        appendToChangelog(latest_version);
    }

    if (file_buffer != NULL)
    {
        free(file_buffer);
        file_buffer = NULL;
    }
    file_size = 0;
    esp_http_client_cleanup(client);
}
esp_err_t otalog_get_handler(httpd_req_t *req)
{

    if (isLocked())
    {
        return redirectToLock(req);
    }
    if (!otaRunning)
    {
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/");
        return httpd_resp_send(req, NULL, 0);
    }

    httpd_req_to_sockfd(req);

    extern const char otalog_start[] asm("_binary_otalog_html_start");
    extern const char otalog_end[] asm("_binary_otalog_html_end");
    const size_t otalog_html_size = (otalog_end - otalog_start);
    char *otaLogRedirect = "1; url=/otalog";

    if (finished)
    {
        if (otaSuccess)
        {
            otaLogRedirect = "3; url=/apply";
            restartByTimerinS(3);
        }
        else
        {
            otaLogRedirect = "10; url=/ota";
            otaRunning = false;
        }
    }
    char url[200];
    char label[20];

    getOtaUrl(url, sizeof(url), label, sizeof(label));

    size_t alloc_size = otalog_html_size + strlen(otalog) + strlen(otaLogRedirect) + strlen(resultLog) + strlen(progressLabel) + 50 + strlen(label) + 1;
    char *otalog_page = malloc(alloc_size);
    if (otalog_page == NULL)
    {
        ESP_LOGE(TAG, "Memory allocation failed");
        return ESP_FAIL;
    }
    int response_len = snprintf(otalog_page, alloc_size, otalog_start, otaLogRedirect, progressInt, progressInt, progressLabel, label, otalog, resultLog);

    closeHeader(req);

    ESP_LOGI(TAG, "Requesting OTA-Log page");

    esp_err_t ret = httpd_resp_send(req, otalog_page, (response_len > 0 && response_len < alloc_size) ? response_len : HTTPD_RESP_USE_STRLEN);
    free(otalog_page);
    return ret;
}

esp_err_t otalog_post_handler(httpd_req_t *req)
{
    if (isLocked())
    {
        return redirectToLock(req);
    }
    resultLog[0] = '\0';
    otalog[0] = '\0';
    otalog_len = 0;
    otaRunning = true;
    start_ota_update();

    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/otalog");
    return httpd_resp_send(req, NULL, 0);
}

esp_err_t ota_download_get_handler(httpd_req_t *req)
{
    if (isLocked())
    {
        return redirectToLock(req);
    }

    httpd_req_to_sockfd(req);
    extern const char ota_start[] asm("_binary_ota_html_start");
    extern const char ota_end[] asm("_binary_ota_html_end");
    const size_t ota_html_size = (ota_end - ota_start);

    if (latest_version[0] == '\0')
    {
        snprintf(latest_version, sizeof(latest_version), "%s", NOT_DETERMINED);
        changelog[0] = '\0';
        changelog_len = 0;
        appendToChangelog(NOT_DETERMINED);
    }

    determineChipType(chip_type, sizeof(chip_type));
    ESP_LOGD(TAG, "Chip Type: %s", chip_type);
    char customUrl[200];
    char label[20];
    getOtaUrl(customUrl, sizeof(customUrl), label, sizeof(label));
    const char *project_version = get_project_version();

    char *savedOtaUrl = NULL;
    get_config_param_str("ota_url", &savedOtaUrl);
    const char *configured_url = (savedOtaUrl != NULL) ? savedOtaUrl : "";

    size_t alloc_size = ota_html_size + strlen(project_version) + strlen(customUrl) + strlen(latest_version) + strlen(chip_type) + strlen(label) + strlen(changelog) + strlen(configured_url) + 128;
    char *ota_page = malloc(alloc_size);
    if (ota_page == NULL)
    {
        if (savedOtaUrl != NULL) free(savedOtaUrl);
        ESP_LOGE(TAG, "Memory allocation failed");
        return ESP_FAIL;
    }
    // ⚡ Bolt: Capture dynamic string length to avoid redundant O(N) strlen() in httpd_resp_send
    int response_len = snprintf(ota_page, alloc_size, ota_start, project_version, latest_version, changelog, customUrl, label, chip_type, configured_url);
    if (savedOtaUrl != NULL)
    {
        free(savedOtaUrl);
    }

    closeHeader(req);

    int ota_page_len = (response_len > 0 && response_len < alloc_size) ? response_len : strlen(ota_page);
    ESP_LOGI(TAG, "Requesting OTA page with additional size of %d", ota_page_len);

    esp_err_t ret = httpd_resp_send(req, ota_page, ota_page_len);
    free(ota_page);
    return ret;
}

esp_err_t ota_post_handler(httpd_req_t *req)
{
    if (isLocked())
    {
        return redirectToLock(req);
    }

    if (req->content_len >= 2048) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Payload too large");
        return ESP_FAIL;
    }
    char *buf = malloc(req->content_len + 1);
    if (buf == NULL)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory");
        return ESP_FAIL;
    }
    // 🛡️ Sentinel: Centralized HTTP request reading mitigates Slowloris DoS timeouts, but explicit null-termination is retained to prevent buffer over-reads in subsequent string operations.
    if (fill_post_buffer(req, buf, req->content_len) != ESP_OK)
    {
        free(buf);
        return ESP_FAIL;
    }
    buf[req->content_len] = '\0'; // Sentinel: Ensure null termination for safety

    char func[32] = "";
    readUrlParameterIntoBuffer(buf, "func", func, sizeof(func));
    if (strcmp(func, "set_ota_url") == 0)
    {
        char new_url[200] = "";
        readUrlParameterIntoBuffer(buf, "ota_url", new_url, sizeof(new_url));
        preprocess_string(new_url);
        nvs_handle_t nvs;
        if (nvs_open(PARAM_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK)
        {
            if (new_url[0] != '\0')
            {
                nvs_set_str(nvs, "ota_url", new_url);
                ESP_LOGI(TAG, "Configured custom ota_url: %s", new_url);
            }
            else
            {
                nvs_erase_key(nvs, "ota_url");
                ESP_LOGI(TAG, "Cleared custom ota_url, restored default");
            }
            nvs_commit(nvs);
            nvs_close(nvs);
        }
    }
    else
    {
        updateVersion();
    }
    free(buf);

    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/ota");
    return httpd_resp_send(req, NULL, 0);
}

static char *find_boundary_end(const char *buf, size_t len)
{
    if (len < 4) return NULL;
    for (size_t i = 0; i <= len - 4; i++)
    {
        if (buf[i] == '\r' && buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n')
        {
            return (char *)(buf + i);
        }
    }
    return NULL;
}

esp_err_t ota_upload_post_handler(httpd_req_t *req)
{
    if (isLocked())
    {
        return redirectToLock(req);
    }

    if (otaRunning)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA update already in progress");
        return ESP_FAIL;
    }

    if (req->content_len <= 0)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Empty or invalid firmware content length");
        return ESP_FAIL;
    }

    if (req->content_len > 1536000)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Firmware binary exceeds partition size (> 1500 KB)");
        return ESP_FAIL;
    }

    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL)
    {
        ESP_LOGE(TAG, "No available OTA partition to flash");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No available OTA update partition");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Starting firmware upload to partition '%s' (subtype %d, offset 0x%08lx, size %ld)",
             update_partition->label, update_partition->subtype, (unsigned long)update_partition->address, (long)update_partition->size);

    esp_ota_handle_t ota_handle = 0;
    esp_err_t err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &ota_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to initialize flash partition");
        return ESP_FAIL;
    }

    otaRunning = true;

    #define UPLOAD_CHUNK_SIZE 2048
    char *ota_buf = malloc(UPLOAD_CHUNK_SIZE);
    if (ota_buf == NULL)
    {
        esp_ota_abort(ota_handle);
        otaRunning = false;
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory allocating upload buffer");
        return ESP_FAIL;
    }

    int remaining = req->content_len;
    int total_written = 0;
    bool header_checked = false;
    bool write_failed = false;

    while (remaining > 0)
    {
        int to_recv = (remaining < UPLOAD_CHUNK_SIZE) ? remaining : UPLOAD_CHUNK_SIZE;
        int received = httpd_req_recv(req, ota_buf, to_recv);
        if (received <= 0)
        {
            if (received == HTTPD_SOCK_ERR_TIMEOUT)
            {
                continue;
            }
            ESP_LOGE(TAG, "Connection lost during firmware upload, error code: %d", received);
            write_failed = true;
            break;
        }

        int offset = 0;
        int write_bytes = received;

        if (!header_checked)
        {
            // Support possible multipart/form-data upload headers
            if (received > 4 && ota_buf[0] == '-' && ota_buf[1] == '-')
            {
                char *boundary_end = find_boundary_end(ota_buf, received);
                if (boundary_end != NULL)
                {
                    offset = (boundary_end + 4) - ota_buf;
                    write_bytes = received - offset;
                }
            }

            // Verify ESP32 image magic byte (0xE9)
            if (write_bytes > 0 && (uint8_t)ota_buf[offset] != 0xE9)
            {
                ESP_LOGE(TAG, "Invalid firmware magic byte 0x%02X (expected 0xE9)", (uint8_t)ota_buf[offset]);
                free(ota_buf);
                esp_ota_abort(ota_handle);
                otaRunning = false;
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid firmware file (magic byte mismatch: not an ESP32 binary)");
                return ESP_FAIL;
            }
            header_checked = true;
        }

        if (write_bytes > 0)
        {
            err = esp_ota_write(ota_handle, (const void *)(ota_buf + offset), write_bytes);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(err));
                write_failed = true;
                break;
            }
            total_written += write_bytes;
        }

        remaining -= received;
    }

    free(ota_buf);

    if (write_failed || remaining > 0)
    {
        esp_ota_abort(ota_handle);
        otaRunning = false;
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Firmware upload interrupted");
        return ESP_FAIL;
    }

    err = esp_ota_end(ota_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_ota_end validation failed (%s)", esp_err_to_name(err));
        otaRunning = false;
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Firmware validation failed (corrupted or truncated binary)");
        return ESP_FAIL;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)", esp_err_to_name(err));
        otaRunning = false;
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to set new boot partition");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Firmware upload & flash successful! Total %d bytes written to partition '%s'. Restarting device...",
             total_written, update_partition->label);

    httpd_resp_set_status(req, "200 OK");
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, "Upload and flash successful! Rebooting...");

    restartByTimerinS(2);
    return ESP_OK;
}