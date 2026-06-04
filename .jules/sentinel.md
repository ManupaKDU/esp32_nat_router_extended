## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2025-02-28 - Double-Free Vulnerability in Request Handlers
**Vulnerability:** A double-free vulnerability existed in `unlock_handler`. The dynamically allocated pointer `unlockParam` was freed inside an `if` block, but also freed again at the end of the function regardless of the control flow.
**Learning:** In C request handlers with multiple conditional branches, memory cleanup must be carefully managed to avoid freeing the same pointer twice, which can lead to memory corruption or arbitrary code execution.
**Prevention:** Strictly centralize memory cleanup at the end of the function's scope, or explicitly set pointer variables to `NULL` immediately after the first `free()` to prevent double-free errors.
