## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
## 2025-02-28 - Memory Leak in Configuration Strings
**Vulnerability:** The `get_config_param_str` function dynamically allocates memory (`malloc`) for the returned string, but callers in URI handlers often fail to `free()` these pointers, leading to memory leaks and a potential Denial of Service (DoS) by exhausting the constrained heap space of the ESP32.
**Learning:** Handlers sometimes reassign these variables to string literals (like `""`) during logic processing. Simply adding `free()` at the end can cause crashes if the variable no longer points to the heap.
**Prevention:** Always capture the original pointer immediately after allocation (e.g., `char *orig_sta = sta;`) and `free(orig_sta)` at the end of the handler, guaranteeing the original heap memory is freed regardless of how the variable is manipulated later.
