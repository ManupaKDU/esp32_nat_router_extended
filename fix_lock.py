import os

filepath = "src/urihandler/lockhandler.c"
with open(filepath, "r") as f:
    content = f.read()

content = content.replace("""    if (req->method == HTTP_POST) // Relock if called
    {
        int ret, remaining = req->content_len;
        char buf[req->content_len + 1];""", """    if (req->method == HTTP_POST) // Relock if called
    {
        if (req->content_len >= 2048) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Payload too large");
            return ESP_FAIL;
        }
        char* buf = malloc(req->content_len + 1);
        if (!buf) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory");
            return ESP_FAIL;
        }
        int ret, remaining = req->content_len;""")


content = content.replace("""        while (remaining > 0)
        {
            /* Read the data for the request */
            if ((ret = httpd_req_recv(req, buf + (req->content_len - remaining), MIN(remaining, sizeof(buf) - 1))) <= 0)
            {
                if (ret == HTTPD_SOCK_ERR_TIMEOUT)
                {
                    continue;
                }
                ESP_LOGE(TAG, "Timeout occured");
                return ESP_FAIL;
            }

            remaining -= ret;
        }
        buf[req->content_len] = '\\0';

        char passParam[req->content_len + 1], pass2Param[req->content_len + 1];

        readUrlParameterIntoBuffer(buf, "lockpass", passParam, req->content_len);
        readUrlParameterIntoBuffer(buf, "lockpass2", pass2Param, req->content_len);""", """        while (remaining > 0)
        {
            /* Read the data for the request */
            if ((ret = httpd_req_recv(req, buf + (req->content_len - remaining), MIN(remaining, req->content_len))) <= 0)
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
        buf[req->content_len] = '\\0';

        char passParam[128], pass2Param[128];

        readUrlParameterIntoBuffer(buf, "lockpass", passParam, sizeof(passParam));
        readUrlParameterIntoBuffer(buf, "lockpass2", pass2Param, sizeof(pass2Param));""")


with open(filepath, "w") as f:
    f.write(content)
