## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2026-05-02 - [HTTPD_RESP_USE_STRLEN Optimization]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` causes the HTTP server to implicitly call `strlen()` on the entire response buffer. For large HTML pages, this is a redundant O(N) operation.
**Action:** Pre-calculate or cache the response length manually and pass it directly to `httpd_resp_send` instead of using the `HTTPD_RESP_USE_STRLEN` macro.
## 2026-05-05 - [Safe snprintf Length Capturing]
**Learning:** While capturing the return value of `snprintf` is an excellent way to get the string length without calling `strlen`, `snprintf` returns the number of characters that *would* have been written if the buffer was large enough, not the actual characters written. Passing this blindly to network functions like `httpd_resp_send` can result in out-of-bounds reads if the buffer was truncated.
**Action:** Always constrain the captured `snprintf` length against the actual allocated buffer size (e.g., `len < size ? len : size - 1`) before using it for network operations or pointer arithmetic.
