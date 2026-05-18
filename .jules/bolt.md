## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2026-05-02 - [HTTPD_RESP_USE_STRLEN Optimization]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` causes the HTTP server to implicitly call `strlen()` on the entire response buffer. For large HTML pages, this is a redundant O(N) operation.
**Action:** Pre-calculate or cache the response length manually and pass it directly to `httpd_resp_send` instead of using the `HTTPD_RESP_USE_STRLEN` macro.
## 2024-05-18 - [HTTPD_RESP_USE_STRLEN Revisit: Dynamic Asset Pre-calculation]
**Learning:** For ESP-IDF embedded binary assets (like CSS and JS files generated from strings with hashes in their filenames), their exact payload sizes can be safely derived by finding the `_start` and `_end` linker symbols from the compiled `.pio/build/esp32/` assembly files. Subtracting `1` from `(_end - _start)` correctly accounts for the `EMBED_TXTFILES` null-terminator byte, avoiding the `strlen` entirely.
**Action:** Always pre-calculate HTTP response payloads lengths natively using linker pointer math instead of using `HTTPD_RESP_USE_STRLEN` to achieve an O(1) performance response. Ensure 0 is explicitly passed for zero-byte responses (e.g., `""`).
