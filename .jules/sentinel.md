## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2025-10-31 - [Double Free Vulnerability in C Request Handlers]
**Vulnerability:** Double free of a heap-allocated pointer (`unlockParam` in `lockhandler.c`). The pointer was freed inside an `if` block during a successful branch, and then erroneously freed again unconditionally at the end of the outer block.
**Learning:** C request handlers with multiple conditional branches and early exits are highly susceptible to double-free vulnerabilities if memory cleanup is duplicated across scopes.
**Prevention:** Strictly centralize memory cleanup at the end of the function's scope, or explicitly set pointer variables to `NULL` immediately after the first `free()` to avoid double free vulnerabilities.
