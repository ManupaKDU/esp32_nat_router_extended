## 2024-03-13 - [Buffer Overflow]
**Vulnerability:** Buffer overflow in HTTP POST request handlers (`applyhandler.c`, `lockhandler.c`, `otahandler.c`, etc.) due to incorrect buffer size allocation for NUL-terminated strings and missing pointer offset logic during chunked reads.
**Learning:** The ESP-IDF `httpd_req_recv` function does not automatically NUL-terminate strings. When reading HTTP request bodies into a buffer, the buffer must be allocated with an extra byte for the NUL terminator, and the pointer must be correctly offset during chunked reads to avoid overwriting the beginning of the buffer.
**Prevention:** Always allocate `req->content_len + 1` for buffers intended to hold NUL-terminated strings. When reading chunked data, use `buf + (len - remaining)` to advance the pointer and ensure the buffer is explicitly NUL-terminated after the read loop (`buf[len] = '\0'`).

## 2026-03-14 - [Buffer Overflow]
**Vulnerability:** Unbounded writes to log buffers in `otahandler.c`. `sprintf` and `strcat` could exceed global array limits (`otalog`, `changelog`, `resultLog`) if an attacker provides a malicious/long OTA URL causing large responses.
**Learning:** Global variables for logging on embedded systems are prone to overflow if appended indiscriminately. When aggregating data from external inputs over HTTP (like release notes/OTA progress), checking string limits before concatenating is crucial.
**Prevention:** Replace bounded `sprintf` and `strcat` functions with `snprintf` and `strncat` checking the destination buffer size before appending.
## 2024-05-30 - [Buffer Overflow in string truncation logic]
 **Vulnerability:** Stack Buffer Overflow in IP address string truncation (`ip_prefix[strlen(defaultIP) - 1] = '\0'`) when allocating exactly `strlen(defaultIP) - 1` space instead of `strlen(defaultIP)`.
 **Learning:** When truncating a string by replacing a character with `\0`, you must ensure the underlying buffer is sized to include the `\0`. If you allocate `strlen(str) - 1` and write `\0` to index `strlen(str) - 1`, you are writing out of bounds by one byte.
 **Prevention:** Allocate `strlen(str)` or more space to safely accommodate the truncated string and its NUL terminator at index `strlen(str) - 1`. Always account for the `\0` byte when calculating buffer sizes.
