## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.## 2024-05-23 - Timing Attack Vulnerability in Password Verification
**Vulnerability:** The lock handler uses `strcmp()` to verify passwords against stored configuration secrets (`lock_pass`).
**Learning:** `strcmp()` terminates early upon finding the first differing character. This creates a measurable difference in execution time depending on how many prefix characters match, which attackers can exploit to brute-force the password character-by-character (Timing Attack / CWE-208).
**Prevention:** Always use constant-time comparison functions (like `crypto_memcmp` implemented with `volatile` pointers) for checking passwords, API keys, or other cryptographic secrets, and ensure length checks occur before the memory comparison.
