## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2026-05-02 - [HTTPD_RESP_USE_STRLEN Optimization]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` causes the HTTP server to implicitly call `strlen()` on the entire response buffer. For large HTML pages, this is a redundant O(N) operation.
**Action:** Pre-calculate or cache the response length manually and pass it directly to `httpd_resp_send` instead of using the `HTTPD_RESP_USE_STRLEN` macro.
## 2026-05-18 - [Embedded Assets & O(1) Length Calculation]
**Learning:** Embedded text files in ESP-IDF (`COMPONENT_EMBED_TXTFILES`) append a null byte at the end of the data. When eliminating `HTTPD_RESP_USE_STRLEN` and manually calculating the size of an embedded file using linker symbols (e.g., `_end` - `_start`), you must subtract 1 `(size_t)(file_end - file_start) - 1` to avoid sending the trailing null byte over the network.
**Action:** When replacing `strlen()` or `HTTPD_RESP_USE_STRLEN` for embedded assets, use `(size_t)(_end - _start) - 1` to correctly compute the size.
