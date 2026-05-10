## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.
## 2024-05-10 - Prevent Buffer Overflows with snprintf (CWE-120)
**Vulnerability:** Widespread use of  without bounds checking in HTTP handlers (e.g., `advancedhandler.c`, `indexhandler.c`) allowed dynamically calculated payloads to potentially overflow allocated heap and stack buffers.
**Learning:** In dynamically scaled HTML generation where inputs are concatenated into templates, `sprintf` creates a significant risk of out-of-bounds writes if the memory estimation logic (like `strlen` sums) fails or if untrusted inputs exceed expected sizes.
**Prevention:** Always use `snprintf` and explicitly pass the allocated buffer size (e.g., `sizeof(buf)` or the `malloc` `size` calculation) to enforce a hard upper limit on string formatting.

## 2024-05-10 - Prevent Buffer Overflows with snprintf (CWE-120)
**Vulnerability:** Widespread use of `sprintf` without bounds checking in HTTP handlers allowed dynamically calculated payloads to potentially overflow allocated heap and stack buffers.
**Learning:** In dynamically scaled HTML generation where inputs are concatenated into templates, `sprintf` creates a significant risk of out-of-bounds writes if the memory estimation logic fails or if untrusted inputs exceed expected sizes.
**Prevention:** Always use `snprintf` and explicitly pass the allocated buffer size (e.g., `sizeof(buf)` or the `malloc` size calculation) to enforce a hard upper limit on string formatting.
