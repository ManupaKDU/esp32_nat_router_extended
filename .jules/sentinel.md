## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2025-02-28 - Double-Free in Request Handlers
**Vulnerability:** A double-free vulnerability (CWE-415) existed in the `unlock_handler` function. An unconditionally executed `free(unlockParam)` occurred after the variable was already freed in the `if` block, leading to memory corruption.
**Learning:** In C request handlers with multiple conditional branches and early returns, it's easy to accidentally free a pointer twice. The ESP-IDF framework does not set freed pointers to `NULL` automatically.
**Prevention:** Strictly centralize memory cleanup at the end of the function's scope, or explicitly set pointer variables to `NULL` immediately after the first `free()`.
