## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.

## 2025-02-28 - Missing Content Length Validation causing DoS
**Vulnerability:** The `unlock_handler` function dynamically allocated memory based on `req->content_len` without first checking if the requested length was excessively large. An attacker could send a request with a massive `Content-Length` header, leading to memory allocation failure or heap exhaustion.
**Learning:** HTTP handlers often use the client-provided `Content-Length` header to determine how much data to read. Since this is user-supplied data, it must be validated before being used to allocate memory on embedded devices with constrained memory.
**Prevention:** Always validate `req->content_len` against a reasonable upper bound (e.g., `< 2048` for simple forms) before attempting dynamic memory allocation (`malloc`) for the request payload.

## 2025-02-28 - Missing Content Length Validation causing DoS in multiple handlers
**Vulnerability:** Similar to the previous finding in `lockhandler.c`, the `index_post_handler`, `portmap_post_handler`, and `apply_post_handler` dynamically allocated memory based on `req->content_len` without first checking if the requested length was excessively large. This could lead to memory allocation failure or heap exhaustion.
**Learning:** HTTP handlers often use the client-provided `Content-Length` header to determine how much data to read. Since this is user-supplied data, it must be validated before being used to allocate memory on embedded devices with constrained memory. The `applyhandler.c` specifically handles form submissions with larger data (like certificates), so it requires a higher limit (8192 bytes) compared to simple form submissions (2048 bytes).
**Prevention:** Always validate `req->content_len` against a reasonable upper bound based on the expected form size (e.g., `< 2048` for simple forms, `< 8192` for forms with larger fields) before attempting dynamic memory allocation (`malloc`) for the request payload.
## 2024-06-13 - Pointer Reassignment Memory Leaks
**Vulnerability:** Memory leaks (DoS risk) caused by un-freed dynamic allocations from configuration retrieval functions (`get_config_param_str` and `get_config_param_blob`) in request handlers.
**Learning:** Handlers often reassigned these dynamically allocated pointers to static strings (e.g., `""`) for logic and UI formatting, losing the reference to the allocated heap block and making it impossible to `free` them at the end of the request handler. This leads to heap exhaustion if requested repeatedly.
**Prevention:** When dynamically allocated configuration parameters might be reassigned during logic flow, explicitly save the original pointer (e.g., `char *orig_sta_identity = sta_identity;`) immediately after allocation to ensure the memory can still be safely freed at all function exits.
## 2024-06-10 - [DoS] Fix missing bounds check on HTTP POST content length
**Vulnerability:** Several HTTP POST request handlers (`applyhandler.c`, `indexhandler.c`, `lockhandler.c`, `portmaphandler.c`) were dynamically allocating buffers using `malloc(req->content_len + 1)` without upper bounding the size of `req->content_len`.
**Learning:** The ESP32 `http_server` framework allows arbitrary values for `content_len` if not bounded. Malicious clients could specify massive payload sizes, leading to immediate memory exhaustion and device crashes (Denial of Service).
**Prevention:** Always validate `req->content_len` against a reasonable limit (e.g., `< 2048`) and reject oversized requests (`HTTPD_400_BAD_REQUEST`) prior to calling `malloc`.
>>>>>>> pr-208
