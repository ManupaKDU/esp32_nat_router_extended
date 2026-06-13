## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2025-02-28 - Unbounded Memory Allocation leading to DoS
**Vulnerability:** Several POST handlers allocated heap memory directly using `req->content_len` without checking if the length was within reasonable bounds.
**Learning:** Blindly trusting `req->content_len` for memory allocation allows an attacker to send requests with massive `Content-Length` headers, causing the device to attempt a huge `malloc`, leading to heap exhaustion and a Denial of Service (DoS) crash.
**Prevention:** Always validate `req->content_len` against a reasonable upper bound (e.g., `< 2048`) before attempting dynamic memory allocation for the request payload. Return `HTTPD_400_BAD_REQUEST` if the size exceeds the bound.
