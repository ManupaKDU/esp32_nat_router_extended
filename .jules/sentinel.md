## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2024-05-15 - Double-Free Vulnerability (CWE-415) & Misleading Logs
**Vulnerability:** A double-free vulnerability exists in `src/urihandler/lockhandler.c` when a user provides a non-empty `unlock` parameter that does not match the stored `lock_pass`.
**Learning:** `free(unlockParam)` was called both inside the `if (strlen(unlockParam) > 0)` block and unconditionally after the block. Additionally, if the string length was 0, a misleading "Memory allocation failed" log was emitted instead of correctly identifying an empty parameter.
**Prevention:** Centralize memory cleanup at the end of the function's scope, or explicitly set pointer variables to `NULL` immediately after the first `free()`. Ensure log messages accurately reflect logic failures vs. heap exhaustion to avoid masking the real issue.
