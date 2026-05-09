## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2026-05-02 - [HTTPD_RESP_USE_STRLEN Optimization]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` causes the HTTP server to implicitly call `strlen()` on the entire response buffer. For large HTML pages, this is a redundant O(N) operation.
**Action:** Pre-calculate or cache the response length manually and pass it directly to `httpd_resp_send` instead of using the `HTTPD_RESP_USE_STRLEN` macro.
## 2024-05-15 - [O(N) strlen in HTTP Response Handler]
**Learning:** By default, using `HTTPD_RESP_USE_STRLEN` in ESP-IDF`s `httpd_resp_send` forces an implicit `strlen()` to determine the payload length. For embedded static files compiled via CMake`s `target_add_binary_data` (like `jquery.js`), this causes thousands of wasted CPU cycles on large payloads where the exact buffer bounds are already exposed by the compiler.
**Action:** When serving embedded static files, avoid `HTTPD_RESP_USE_STRLEN`. Instead, explicitly declare the `_end` linker symbol (e.g., `extern const char file_end[] asm("_binary_file_end");`) and calculate the exact response length in O(1) time using pointer arithmetic `(file_end - file_start) - 1`.
