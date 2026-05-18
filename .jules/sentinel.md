## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.## 2024-05-18 - Prevent Timing Attacks in Password Verification
**Vulnerability:** Timing attacks on password comparison (`strcmp`).
**Learning:** Using `strcmp` for sensitive secrets allows attackers to measure the time it takes to compare strings, potentially deducing the password character by character because `strcmp` short-circuits.
**Prevention:** Always use constant-time comparison functions like `crypto_memcmp` for passwords or sensitive data, ensuring strings are of equal length before comparison.
