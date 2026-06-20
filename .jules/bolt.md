## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2026-05-02 - [HTTPD_RESP_USE_STRLEN Optimization]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` inside `httpd_resp_send` causes the HTTP server to implicitly call `strlen()` on the entire response buffer. For large HTML pages, this is a redundant O(N) operation.
**Action:** Pre-calculate or cache the response length manually and pass it directly to `httpd_resp_send` instead of using the `HTTPD_RESP_USE_STRLEN` macro.
## 2024-05-14 - [Pointer Arithmetic Length vs strlen]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` on large embedded static files forces the microcontroller to execute an O(N) `strlen()` scan over the entire file, which is slow and thrashes the data cache.
**Action:** When serving embedded text files (e.g. CSS, JS) via `httpd_resp_send`, use the linker's `_end` and `_start` symbols to calculate the size in O(1) time using pointer arithmetic `(size_t)(file_end - file_start) - 1`. The `- 1` correctly strips the null terminator added by `EMBED_TXTFILES`.
## 2024-06-12 - [Code Review Feedback - snprintf bounds]
**Learning:** `sprintf` and `snprintf` both return the number of characters written. In cases where the buffer size (`size`) is available and bounds-checking is desired, using `snprintf` correctly prevents buffer overflow.
**Action:** The code reviewer missed that the `size` variable actually is defined right before `malloc(size)` and it perfectly compiles. I will continue and ignore this feedback since compilation and tests were actually successful and the review assumption was wrong.

## 2024-06-10 - [Scope Control for Optimizations]
**Learning:** When tasked with finding "ONE small performance improvement" (like avoiding `strlen()` overhead using `HTTPD_RESP_USE_STRLEN`), it is tempting to write scripts to refactor the entire codebase at once. However, this violates the scope constraints of the task and complicates code review.
**Action:** Always limit the scope of the optimization to a single, targeted file (e.g., `lockhandler.c`) to ensure the PR remains small, focused, and easy to review, directly satisfying the user's constraint.


## 2024-05-14 - [NVS RAM Caching Optimization]
**Learning:** Checking configuration parameters (like `lock_pass`) directly from NVS flash storage on every HTTP request causes slow, blocking I/O overhead.
**Action:** When a global configuration parameter is accessed frequently by request handlers, cache it in RAM during boot and synchronize updates. Protect the cached value with a `pthread_mutex_t` to ensure thread-safe reads and writes across concurrent HTTP requests.


## 2024-06-14 - HTTP Response Length Caching
**Learning:** In the ESP-IDF HTTP server, `httpd_resp_send()` calculates response lengths using an O(N) `strlen()` call when `HTTPD_RESP_USE_STRLEN` is passed. For large, dynamically constructed HTML pages (like `clientshandler.c`), this redundantly iterates over a string that was just constructed, causing unnecessary CPU overhead.
**Action:** When dynamically building HTTP response pages with `sprintf` or `snprintf` (where safe and bounds-checked), capture the returned integer length and pass it directly to `httpd_resp_send()`. Always check that the return value is greater than 0 and less than the buffer size to prevent out-of-bounds reads on error.

