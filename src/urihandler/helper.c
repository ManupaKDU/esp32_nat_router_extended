#include "helper.h"
#include <ctype.h>

static const char *TAG = "urihelper";

void preprocess_string(char *str)
{
    char *p, *q;

    for (p = q = str; *p != 0; p++)
    {
        if (*(p) == '%' && *(p + 1) != 0 && *(p + 2) != 0)
        {
            // quoted hex
            uint8_t a;
            p++;
            if (*p <= '9')
                a = *p - '0';
            else
                a = toupper((unsigned char)*p) - 'A' + 10;
            a <<= 4;
            p++;
            if (*p <= '9')
                a += *p - '0';
            else
                a += toupper((unsigned char)*p) - 'A' + 10;
            *q++ = a;
        }
        else if (*(p) == '+')
        {
            *q++ = ' ';
        }
        else
        {
            *q++ = *p;
        }
    }
    *q = '\0';
}

void readUrlParameterIntoBuffer(char *parameterString, char *parameter, char *buffer, size_t paramLength)
{
    if (httpd_query_key_value(parameterString, parameter, buffer, paramLength) == ESP_OK)
    {
        preprocess_string(buffer);
        if (strstr(parameter, "pass") != NULL ||
            strstr(parameter, "unlock") != NULL ||
            strstr(parameter, "user") != NULL ||
            strstr(parameter, "identity") != NULL) {
            ESP_LOGI(TAG, "Found '%s' parameter => ***REDACTED***", parameter);
        } else {
            ESP_LOGI(TAG, "Found '%s' parameter => %s", parameter, buffer);
        }
    }
    else
    {
        ESP_LOGI(TAG, "Parameter '%s' not found", parameter);
        buffer[0] = '\0';
    }
}

esp_err_t fill_post_buffer(httpd_req_t *req, char *buf, size_t len)
{
    int ret, remaining = len;

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf + (len - remaining), MIN(remaining, len))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                continue;
            }
            ESP_LOGE(TAG, "Timeout occurred");
            return ESP_FAIL;
        }

        remaining -= ret;
    }

    buf[len] = '\0';
    return ESP_OK;
}

bool is_valid_subnet_mask(char *subnet_mask)
{
    ip_addr_t ip_addr;

    // ⚡ Bolt: Use ipaddr_aton to validate directly instead of expensive string copying and multiple strtok/atoi calls
    if (!ipaddr_aton(subnet_mask, &ip_addr))
    {
        ESP_LOGE(TAG, "%s is not a valid subnet mask. Invalid format.", subnet_mask);
        return false;
    }

    // ipaddr_aton parses the string correctly. Get the 32-bit integer in host byte order.
    u_int32_t num = ntohl(ip4_addr_get_u32(ip_2_ip4(&ip_addr)));

    unsigned int inverted_mask = ~num;
    if ((inverted_mask & (inverted_mask + 1)) != 0)
    {
        ESP_LOGE(TAG, "%s is not a valid subnet mask. The bits after the last 1 have to be zero.", subnet_mask);
        return false;
    }

    return true;
}