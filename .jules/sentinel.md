## 2025-02-28 - Timing Attack in Password Check
**Vulnerability:** The lock handler used `strcmp` to compare passwords. This allows an attacker to deduce the password character by character by measuring the time it takes for the comparison to fail.
**Learning:** Standard C string comparison functions like `strcmp` short-circuit and return immediately upon finding a difference. This creates a timing side-channel.
**Prevention:** Sensitive data like passwords or tokens should always be compared using a constant-time comparison function, such as `crypto_memcmp`. First check that the lengths are equal (to prevent other types of attacks), and then use `crypto_memcmp` to check the contents.
