## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2026-05-02 - [HTTPD_RESP_USE_STRLEN Optimization]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` causes the HTTP server to implicitly call `strlen()` on the entire response buffer. For large HTML pages, this is a redundant O(N) operation.
**Action:** Pre-calculate or cache the response length manually and pass it directly to `httpd_resp_send` instead of using the `HTTPD_RESP_USE_STRLEN` macro.
## 2024-05-14 - [Pointer Arithmetic Length vs strlen]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` on large embedded static files forces the microcontroller to execute an O(N) `strlen()` scan over the entire file, which is slow and thrashes the data cache.
**Action:** When serving embedded text files (e.g. CSS, JS) via `httpd_resp_send`, use the linker's `_end` and `_start` symbols to calculate the size in O(1) time using pointer arithmetic `(size_t)(file_end - file_start) - 1`. The `- 1` correctly strips the null terminator added by `EMBED_TXTFILES`.
## 2024-05-25 - [snprintf return value caching for httpd_resp_send]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` forces an O(N) `strlen()` call on the buffer. When dynamic strings are constructed using `snprintf`, the function returns the number of characters formatted. Discarding this return value forces the system to redundantly calculate the string length.
**Action:** Capture the return value of `snprintf` when preparing dynamic HTML buffers. Bounds check it (e.g., `(len > 0 && len < size) ? len : size - 1`) and pass this validated length directly to `httpd_resp_send` instead of relying on `HTTPD_RESP_USE_STRLEN`.
