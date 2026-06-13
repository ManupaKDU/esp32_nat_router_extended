## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## $(date +%Y-%m-%d) - Prevent DoS via Heap Exhaustion in get_config_param handlers
**Vulnerability:** Memory leaks (CWE-400) existed in request handlers (e.g., indexhandler.c) because dynamically allocated pointers from `get_config_param_str` and `get_config_param_blob` were never passed to `free()`.
**Learning:** Reassigning these pointers (e.g., to `""`) inside the handler logic caused the original heap address to be lost, making it impossible to `free()` and leading to progressive heap exhaustion on repeated HTTP requests.
**Prevention:** Always create a copy of the original pointer immediately after dynamic allocation (e.g., `char *orig_sta_identity = sta_identity;`) if reassignment is possible, and ensure the original pointers are explicitly freed at the end of the handler scope and before any early returns.
