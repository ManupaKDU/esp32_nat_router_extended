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

## 2025-02-28 - Unbounded Memory Allocation leading to DoS
**Vulnerability:** Several POST handlers allocated heap memory directly using `req->content_len` without checking if the length was within reasonable bounds.
**Learning:** Blindly trusting `req->content_len` for memory allocation allows an attacker to send requests with massive `Content-Length` headers, causing the device to attempt a huge `malloc`, leading to heap exhaustion and a Denial of Service (DoS) crash.
**Prevention:** Always validate `req->content_len` against a reasonable upper bound (e.g., `< 2048`) before attempting dynamic memory allocation for the request payload. Return `HTTPD_400_BAD_REQUEST` if the size exceeds the bound.


## 2025-02-28 - Missing Content Length Validation in POST Handlers
**Vulnerability:** Several POST handlers (`index_post_handler`, `apply_post_handler`, `unlock_handler`, `portmap_post_handler`) allocated memory based on `req->content_len` without checking if it exceeded a reasonable maximum length. This allows attackers to send requests with extremely large payloads, potentially causing memory allocation failures, leading to Denial of Service (DoS) via heap exhaustion.
**Learning:** Request handlers must always validate client-supplied length boundaries before relying on them for dynamic memory allocations. The pattern previously established in `lock_handler.c` (`if (req->content_len >= 2048)`) must be applied consistently to all endpoints receiving POST payloads.
**Prevention:** Whenever allocating memory for a request payload based on `req->content_len`, always insert a max-bounds check (`if (req->content_len >= 2048) { return HTTPD_400_BAD_REQUEST; }`) before the `malloc` call.


## 2026-06-20 - Prevent DoS via Heap Exhaustion in get_config_param handlers
**Vulnerability:** Memory leaks (CWE-400) existed in request handlers (e.g., indexhandler.c) because dynamically allocated pointers from `get_config_param_str` and `get_config_param_blob` were never passed to `free()`.
**Learning:** Reassigning these pointers (e.g., to `""`) inside the handler logic caused the original heap address to be lost, making it impossible to `free()` and leading to progressive heap exhaustion on repeated HTTP requests.
**Prevention:** Always create a copy of the original pointer immediately after dynamic allocation (e.g., `char *orig_sta_identity = sta_identity;`) if reassignment is possible, and ensure the original pointers are explicitly freed at the end of the handler scope and before any early returns.


## 2025-02-28 - Memory Leak in Handlers (DoS Risk)
**Vulnerability:** Several HTTP handlers called `get_config_param_str` which internally allocated strings via `malloc`, but failed to call `free()` before returning. This led to a heap memory leak on every request.
**Learning:** Functions that allocate and return memory, like `get_config_param_str`, must have their corresponding `free()` calls in the calling context to prevent memory exhaustion and DoS attacks.
**Prevention:** Always verify the memory lifecycle of functions returning strings or structs, and explicitly call `free()` for all dynamically allocated memory in handlers before the HTTP response completes.


## 2024-06-06 - Memory Leaks in Configuration Retrieval
**Vulnerability:** Memory leaks (and potential DoS) in handlers that call `get_config_param_str` and `get_config_param_blob`. These functions dynamically allocate memory that the caller must free. Many handlers across the codebase (e.g., `indexhandler.c`, `lockhandler.c`, `resulthandler.c`, `esp32_nat_router.c`) failed to `free()` the returned pointers.
**Learning:** Functions that allocate and return memory implicitly transfer ownership and responsibility for cleanup to the caller. This is a common source of memory leaks in C. If these are configuration parameters accessed on every request, it will eventually exhaust heap memory.
**Prevention:** When using custom helper functions that retrieve data, carefully check their implementation to determine if they allocate memory dynamically. If they do, always ensure a corresponding `free()` is called in all execution paths of the calling function, especially before returning. Use proxy variables to hold original pointers if the local pointer variables might be reassigned to static strings.

