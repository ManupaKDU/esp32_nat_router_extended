## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.
## 2024-05-17 - Prevent Timing Attacks on Passwords (CWE-208)
**Vulnerability:** Passwords in `lockhandler.c` were being compared using `strcmp()`, making the system vulnerable to timing attacks where an attacker could deduce the password character by character based on response times.
**Learning:** Standard C string comparison functions like `strcmp` return as soon as a mismatch is found, exposing a side-channel. Security checks should never use `strcmp` for validating secrets.
**Prevention:** Implement and use a constant-time memory comparison function, `crypto_memcmp()`, that evaluates all characters to neutralize timing discrepancies when handling sensitive data.
