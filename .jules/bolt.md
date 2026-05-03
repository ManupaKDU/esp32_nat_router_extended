## 2024-05-03 - O(N) Redundant strlen eliminated for HTTP response payloads
**Learning:** Calling `httpd_resp_send()` with the `HTTPD_RESP_USE_STRLEN` macro forces the ESP-IDF HTTP server to internally call `strlen()` to determine payload length. When the payload string was immediately constructed by `sprintf()` or `snprintf()`, this introduces an unnecessary O(N) traversal.
**Action:** Always capture the integer return value of `sprintf()` and `snprintf()`, which natively returns the exact string length (excluding the null terminator), and pass that cached length instead of `HTTPD_RESP_USE_STRLEN` or redundant `strlen()` calls to any HTTP responses or logging statements.

## 2024-05-03 - ESP-IDF v5 Migration for UART VFS
**Learning:** The UART VFS API has changed in ESP-IDF v5. The header `<driver/uart_vfs.h>` is replaced by `<esp_vfs_dev.h>`, and the functions `uart_vfs_dev_port_set_rx_line_endings`, `uart_vfs_dev_port_set_tx_line_endings`, and `uart_vfs_dev_use_driver` are renamed with the `esp_vfs_dev_uart_` prefix.
**Action:** When migrating ESP-IDF applications to v5+, update the UART VFS headers and function names accordingly to avoid build failures due to implicit function declarations.
