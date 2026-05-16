## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.
## 2024-05-24 - Double-Free in lockhandler
**Vulnerability:** Double-free (CWE-415) on `unlockParam` in `unlock_handler` and misleading error logs that confused memory allocation failures with normal validation failures.
**Learning:** In C handlers with multiple conditional branches and early returns, it's easy to accidentally duplicate memory cleanup (e.g. `free()`). The logs also didn't reflect the validation logic correctly, leading to potential masking of actual attacks or memory exhaustion.
**Prevention:** Strictly centralize memory cleanup or use `NULL` assignments after the first `free()`. Also, ensure logs accurately reflect authentication/validation logic rather than generic memory errors.
