## 2024-03-23 - Missing +1 for NUL Terminator in VLA Allocations
 **Vulnerability:** Variable Length Arrays (VLAs) or dynamically allocated buffers used for string concatenation (e.g., `strcpy` + `strcat` or `sprintf`) frequently lack the `+ 1` required for the NUL terminator. This was seen in `redirectToRoot` in `statichandler.c`, where `char str[strlen("http://") + strlen(currentIP)]` caused an off-by-one buffer overflow.
 **Learning:** In C, functions like `strlen()` do not count the terminating `\0` byte. When allocating memory for a string that will be built via `strcpy`/`strcat` or `sprintf`, you must explicitly allocate space for the NUL terminator.
 **Prevention:** Always add `+ 1` when sizing buffers dynamically using `strlen(...)` to account for the NUL terminator. Also, consider using safer functions like `snprintf` with `sizeof` if the buffer size is known or bounded.
## 2024-05-18 - [VLA Denial of Service in HTTP Request Handlers]
 **Vulnerability:** Unbounded Variable Length Array (VLA) allocated on the stack using `req->content_len` in `ota_post_handler`.
 **Learning:** In ESP-IDF, HTTP handlers frequently use limited stack sizes. Relying on an untrusted `Content-Length` header for stack allocations (`char buf[req->content_len + 1];`) introduces a critical stack overflow and Denial of Service (DoS) vulnerability.
 **Prevention:** Always use dynamic heap allocation (`malloc`) for arbitrary-sized HTTP request bodies. Ensure proper `NULL` checks are added, and the memory is freed on all exit paths (success and error/timeout) to prevent memory leaks.
