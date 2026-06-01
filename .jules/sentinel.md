## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.

## 2025-02-28 - Double-Free Vulnerability in lockhandler.c
**Vulnerability:** A double-free bug existed in `unlock_handler` where `unlockParam` was freed inside an `if` block upon a failed password comparison, and then unconditionally freed again at the end of the function.
**Learning:** In C request handlers with multiple conditional exit points, manual memory management can easily lead to memory corruption (CWE-415) if cleanup is duplicated across logical branches.
**Prevention:** Strictly centralize memory cleanup at the end of the function's scope, or explicitly set pointer variables to `NULL` immediately after the first `free()` to render subsequent frees safe (as `free(NULL)` is a no-op).
