## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2026-05-02 - [HTTPD_RESP_USE_STRLEN Optimization]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` causes the HTTP server to implicitly call `strlen()` on the entire response buffer. For large HTML pages, this is a redundant O(N) operation.
**Action:** Pre-calculate or cache the response length manually and pass it directly to `httpd_resp_send` instead of using the `HTTPD_RESP_USE_STRLEN` macro.
## 2024-05-19 - Pre-calculate static asset lengths for httpd_resp_send
**Learning:** Sending embedded static assets (like large CSS or JS files) using `httpd_resp_send` with `HTTPD_RESP_USE_STRLEN` triggers a full `strlen()` pass over the file, which runs in O(N) time.
**Action:** Always pre-calculate the file size using pointer arithmetic on the linker symbols (`(size_t)(file_end - file_start) - 1`) and pass this explicit length to `httpd_resp_send`, eliminating the performance overhead of `strlen()` for large assets.
