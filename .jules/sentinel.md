## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2025-02-28 - Double-Free Vulnerability in HTTP Handlers
**Vulnerability:** A double-free vulnerability existed in `unlock_handler` where `unlockParam` was freed inside a conditional block (`if (strlen(unlockParam) > 0)`) and then unconditionally freed again at the end of the enclosing block.
**Learning:** In HTTP request handlers with complex conditional logic, explicitly tracking memory ownership is critical. Redundant `free()` calls can lead to heap corruption and remote code execution vulnerabilities, especially when handling user-provided POST payloads.
**Prevention:** Strictly centralize memory cleanup at the end of the function's scope, or explicitly set pointer variables to `NULL` immediately after the first `free()` to prevent double-free bugs.
