## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.## 2024-05-20 - [Fix timing attack in password validation]
**Vulnerability:** Timing attack vulnerability (CWE-208) in password verification (`strcmp(lock, unlockParam) == 0`).
**Learning:** `strcmp` compares characters one by one and returns immediately upon finding a difference, making the execution time dependent on the number of matching characters. This allows an attacker to deduce the password by measuring response times.
**Prevention:** Always use a constant-time comparison function, like `crypto_memcmp`, to compare sensitive data such as passwords. Verify string lengths are equal before calling it.
