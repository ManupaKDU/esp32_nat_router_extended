## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2024-06-07 - Missing content_len validation before malloc
**Vulnerability:** HTTP POST request handlers (`apply_post_handler`, `index_post_handler`, `portmap_post_handler`, `unlock_handler`) dynamically allocated memory (`malloc`) based on `req->content_len` without checking if it exceeded a reasonable upper bound.
**Learning:** This exposes the application to a Denial of Service (DoS) vulnerability via heap exhaustion. An attacker could send a request with a massive `Content-Length` header, causing the system to run out of memory and crash.
**Prevention:** Always validate `req->content_len` against an expected upper bound (e.g., `2048`) and reject the request with a `400 Bad Request` before attempting dynamic memory allocation.
