## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.

## 2025-02-28 - Missing Content Length Validation in POST Handlers
**Vulnerability:** Several POST handlers (`index_post_handler`, `apply_post_handler`, `unlock_handler`, `portmap_post_handler`) allocated memory based on `req->content_len` without checking if it exceeded a reasonable maximum length. This allows attackers to send requests with extremely large payloads, potentially causing memory allocation failures, leading to Denial of Service (DoS) via heap exhaustion.
**Learning:** Request handlers must always validate client-supplied length boundaries before relying on them for dynamic memory allocations. The pattern previously established in `lock_handler.c` (`if (req->content_len >= 2048)`) must be applied consistently to all endpoints receiving POST payloads.
**Prevention:** Whenever allocating memory for a request payload based on `req->content_len`, always insert a max-bounds check (`if (req->content_len >= 2048) { return HTTPD_400_BAD_REQUEST; }`) before the `malloc` call.
