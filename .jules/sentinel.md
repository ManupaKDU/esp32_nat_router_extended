## 2024-05-01 - Prevent Credential Exposure in Logs (CWE-532)
**Vulnerability:** Plaintext passwords (`pass2Param`) and user credentials (`set_sta_ent_arg.user`) were being printed to logs via `ESP_LOGI` formatting.
**Learning:** Even internal logging mechanisms like `ESP_LOGI` can act as sinks for credential exposure, which breaks the required security posture by leaking sensitive configuration values to UART/console.
**Prevention:** Explicitly redact values for keys containing keywords such as `pass`, `user`, `unlock`, or `identity` before they hit the logging functions. Replace the format string variable with static `***REDACTED***` placeholders.
## 2024-05-07 - Stack Exhaustion via VLA on HTTP Inputs (CWE-400)
**Vulnerability:** Variable Length Arrays (VLAs) like `char resultIP[strlen(defaultIP) + strlen(param) + 2];` were allocated directly on the stack using unvalidated HTTP parameter lengths.
**Learning:** In memory-constrained environments like ESP32, allowing untrusted external input (like URL parameters) to dictate stack allocation size without bounds checking creates a trivial Denial of Service (DoS) vector by exhausting the task stack, leading to a system crash.
**Prevention:** Avoid VLAs entirely when handling external input. Use fixed-size stack buffers defined by domain constraints (e.g., `char buffer[16]` for an IPv4 address) and enforce bounds using `snprintf(buf, sizeof(buf), ...)`.
