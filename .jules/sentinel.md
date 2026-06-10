## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2024-06-10 - [DoS] Fix missing bounds check on HTTP POST content length
**Vulnerability:** Several HTTP POST request handlers (`applyhandler.c`, `indexhandler.c`, `lockhandler.c`, `portmaphandler.c`) were dynamically allocating buffers using `malloc(req->content_len + 1)` without upper bounding the size of `req->content_len`.
**Learning:** The ESP32 `http_server` framework allows arbitrary values for `content_len` if not bounded. Malicious clients could specify massive payload sizes, leading to immediate memory exhaustion and device crashes (Denial of Service).
**Prevention:** Always validate `req->content_len` against a reasonable limit (e.g., `< 2048`) and reject oversized requests (`HTTPD_400_BAD_REQUEST`) prior to calling `malloc`.
