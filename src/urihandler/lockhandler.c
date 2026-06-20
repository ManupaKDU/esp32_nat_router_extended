#include "handler.h"
#include "timer.h"

#include <sys/param.h>

#include "nvs.h"
#include "router_globals.h"

static const char *TAG = "LockHandler";

bool locked = false;

bool isLocked()
{
    return locked;
}
void lockUI()
{
    locked = true;
}
esp_err_t unlock_handler(httpd_req_t *req)
{

    httpd_req_to_sockfd(req);

    if (req->content_len >= 2048) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Payload too large");
        return ESP_FAIL;
    }
    size_t content_len = req->content_len;
    if (content_len >= 2048) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Payload too large");
        return ESP_FAIL;
    }
    char *buf = malloc(content_len + 1);
    if (buf == NULL)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory");
        return ESP_FAIL;
    }

    if (fill_post_buffer(req, buf, content_len) == ESP_OK)
    {

        char *unlockParam = malloc(req->content_len + 1);
        if (unlockParam == NULL)
        {
            ESP_LOGE(TAG, "Memory allocation failed");
            free(buf);
            return ESP_FAIL;
        }
        readUrlParameterIntoBuffer(buf, "unlock", unlockParam, req->content_len);

        if (strlen(unlockParam) > 0)
        {
            if (check_lock_pass(unlockParam))
            {
                locked = false;
                httpd_resp_set_status(req, "302 Found");
                httpd_resp_set_hdr(req, "Location", "/");
                free(unlockParam);
                free(buf);
                return httpd_resp_send(req, NULL, 0);
            }
            free(unlockParam);
            unlockParam = NULL;
        }
        else
        {
            ESP_LOGE(TAG, "Memory allocation failed for unlockParam");
        }
        free(unlockParam);
    }
    free(buf);
    if (req->method == HTTP_GET) // Relock if called
    {
        locked = true;
        ESP_LOGI(TAG, "UI relocked");
    }
    extern const char ul_start[] asm("_binary_unlock_html_start");
    extern const char ul_end[] asm("_binary_unlock_html_end");
    const size_t ul_html_size = (ul_end - ul_start) - 1;

    closeHeader(req);
    // ⚡ Bolt: Eliminate O(N) strlen() overhead by passing pre-calculated size
    return httpd_resp_send(req, ul_start, ul_html_size);
}

esp_err_t redirectToLock(httpd_req_t *req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/unlock");
    return httpd_resp_send(req, NULL, 0);
}
esp_err_t lock_handler(httpd_req_t *req)
{
    if (locked)
    {
        return redirectToLock(req);
    }
    httpd_req_to_sockfd(req);

    if (req->method == HTTP_POST) // Relock if called
    {
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
        int ret, remaining = req->content_len;

        while (remaining > 0)
        {
            /* Read the data for the request */
            if ((ret = httpd_req_recv(req, buf + (req->content_len - remaining), MIN(remaining, req->content_len + 1 - (req->content_len - remaining) - 1))) <= 0)
            {
                if (ret == HTTPD_SOCK_ERR_TIMEOUT)
                {
                    continue;
                }
                ESP_LOGE(TAG, "Timeout occured");
                free(buf);
                return ESP_FAIL;
            }

            remaining -= ret;
        }
        buf[req->content_len] = '\0';

        char *passParam = malloc(req->content_len + 1);
        if (passParam == NULL)
        {
            ESP_LOGE(TAG, "Memory allocation failed");
            free(buf);
            return ESP_FAIL;
        }
        char *pass2Param = malloc(req->content_len + 1);
        if (pass2Param == NULL)
        {
            ESP_LOGE(TAG, "Memory allocation failed");
            free(passParam);
            free(buf);
            return ESP_FAIL;
        }

        readUrlParameterIntoBuffer(buf, "lockpass", passParam, req->content_len);
        readUrlParameterIntoBuffer(buf, "lockpass2", pass2Param, req->content_len);
        if (strlen(passParam) == strlen(pass2Param) && crypto_memcmp(passParam, pass2Param, strlen(passParam)) == 0)
        {
            readUrlParameterIntoBuffer(buf, "lockpass", passParam, req->content_len);
            readUrlParameterIntoBuffer(buf, "lockpass2", pass2Param, req->content_len);
            ESP_LOGI(TAG, "Found pass2 parameter => ***REDACTED***");
            if (strlen(passParam) == strlen(pass2Param) && crypto_memcmp(passParam, pass2Param, strlen(passParam)) == 0)
            {
                ESP_LOGI(TAG, "Passes are equal. Password will be changed.");
                if (strlen(passParam) == 0)
                {
                    ESP_LOGI(TAG, "Pass will be removed");
                }
                nvs_handle_t nvs;
                nvs_open(PARAM_NAMESPACE, NVS_READWRITE, &nvs);
                nvs_set_str(nvs, "lock_pass", passParam);
                nvs_commit(nvs);
                nvs_close(nvs);
                update_lock_pass(passParam);
                httpd_resp_set_status(req, "302 Found");
                if (strlen(passParam) > 0)
                {
                    httpd_resp_set_hdr(req, "Location", "/lock");
                    lockUI();
                }
                else
                {
                    httpd_resp_set_hdr(req, "Location", "/");
                }
                free(passParam);
                free(pass2Param);
                free(buf);
                return httpd_resp_send(req, NULL, 0);
            }
            else
            {
                ESP_LOGI(TAG, "Passes are not equal.");
            }
            free(passParam);
            free(pass2Param);
            free(buf);
            return httpd_resp_send(req, NULL, 0);
        }
        else
        {
            ESP_LOGE(TAG, "Memory allocation failed for pass parameters");
        }
        free(passParam);
        free(pass2Param);
        free(buf);
    }

    extern const char l_start[] asm("_binary_lock_html_start");
    extern const char l_end[] asm("_binary_lock_html_end");
    const size_t l_html_size = (l_end - l_start);

    char *display = NULL;

    if (is_lock_pass_set())
    {
        display = "block";
    }
    else
    {
        display = "none";
    }

    size_t alloc_size = l_html_size + strlen(display) + 1;
    char *lock_page = malloc(alloc_size);

    int response_len = snprintf(lock_page, alloc_size, l_start, display);

    closeHeader(req);

    esp_err_t out = httpd_resp_send(req, lock_page, (response_len > 0 && response_len < alloc_size) ? response_len : HTTPD_RESP_USE_STRLEN);
    free(lock_page);
    if (lock_pass != NULL) {
        free(lock_pass);
    }

    return out;
}