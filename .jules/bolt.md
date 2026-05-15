## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2026-05-02 - [HTTPD_RESP_USE_STRLEN Optimization]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` causes the HTTP server to implicitly call `strlen()` on the entire response buffer. For large HTML pages, this is a redundant O(N) operation.
**Action:** Pre-calculate or cache the response length manually and pass it directly to `httpd_resp_send` instead of using the `HTTPD_RESP_USE_STRLEN` macro.
## 2024-05-15 - [Embedded Static File Length Optimization]
**Learning:** For embedded static files (like CSS, JS, HTML), calculating the response length using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` adds a redundant O(N) string length calculation.
**Action:** Use pointer arithmetic with the linker-generated `_start` and `_end` symbols (e.g., `_end - _start`) to pre-calculate the exact file size at compile-time and pass it directly to `httpd_resp_send`.
