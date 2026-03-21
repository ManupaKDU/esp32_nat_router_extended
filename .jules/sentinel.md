## 2024-03-20 - Prevent Credential Leakage in Application Logs
**Vulnerability:** Plaintext Wi-Fi passwords, UI lock passwords, and WPA2 enterprise identities/usernames were being exposed in system logs via `ESP_LOGI` statements across multiple files (`esp32_nat_router.c`, `http_server.c`, `applyhandler.c`).
**Learning:** These logging statements were likely added for debugging purposes during development but left in production code. Since this firmware runs on a device that may output logs over serial or to external services, exposing credentials in plaintext represents a critical security risk.
**Prevention:** Never log sensitive data (passwords, tokens, usernames) in plaintext. When logging connection attempts or configuration updates, either omit the sensitive field entirely or redact it (e.g., using `***`). Regularly audit `ESP_LOG` statements for potential credential leakage.
## 2024-05-18 - Variable Length Arrays (VLA) in HTTP handlers
**Vulnerability:** Several HTTP request handlers allocated arrays on the stack dynamically based on unbounded `req->content_len` headers (e.g., `char buf[req->content_len + 1];`). This poses a stack overflow risk if an attacker sends an artificially large Content-Length.
**Learning:** In constrained devices with limited stack space like the ESP32, dynamic unbounded stack allocations using Variable Length Arrays (VLAs) provide an attack vector for memory corruption and denial-of-service.
**Prevention:** Avoid VLAs dependent on user input. Use fixed-size stack bounds limits. Add validation to reject payloads larger than the limits.
