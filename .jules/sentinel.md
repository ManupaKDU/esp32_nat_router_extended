## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.
## 2024-05-22 - [Timing Attack in Password Verification]
**Vulnerability:** Comparing password inputs and unlock parameters using `strcmp()`.
**Learning:** `strcmp()` compares character by character and returns upon the first mismatch. An attacker could measure the time taken to respond to infer the correctness of individual characters, eventually guessing the whole password. This is a classic CWE-208 vulnerability.
**Prevention:** Always compare sensitive data using constant-time string comparison functions like `crypto_memcmp()`, preceded by a string length check if applicable to ensure bounds correctness without revealing the string match duration.
