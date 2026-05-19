## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.
## 2025-02-23 - Prevent Timing Attack in Password Validation (CWE-208)
**Vulnerability:** The lock handler used `strcmp` to compare user input passwords against the configured lock password. Since `strcmp` evaluates character-by-character and returns early upon mismatch, it execution time varies depending on the number of correct characters, making it susceptible to timing attacks.
**Learning:** For authentication or any logic comparing secrets, standard string comparison functions are a security risk due to data-dependent timing variations.
**Prevention:** Use a constant-time comparison function, such as `crypto_memcmp`, which compares all bytes irrespective of match status. Ensure lengths are compared first before calling it to prevent out-of-bounds reads.
