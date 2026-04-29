## 2024-05-27 - Apply Handler Stack Overflow via User-Controlled VLAs
**Vulnerability:** In `src/urihandler/applyhandler.c`, `char content[bufferLength + 1]` was dynamically allocated on the stack using a Variable-Length Array (VLA) where `bufferLength` was derived directly from the user-controlled HTTP header `req->content_len`. A maliciously large `Content-Length` would cause an immediate stack overflow and remote DoS crash. A similar issue existed in `setApByQuery`, `setStaByQuery`, and `setWpa2` where `char param[contentLength]` was derived from the size of the request string.
**Learning:** ESP32 web server handlers must never trust the HTTP `Content-Length` header for stack allocation sizing. Task stacks are extremely constrained (typically < 8KB), making them trivially susceptible to DoS.
**Prevention:** Always allocate user-controlled buffer sizes on the heap using `malloc()`. Verify that the pointer is not `NULL` before proceeding, as heap allocations frequently fail under memory pressure on embedded devices, and explicitly `free()` the memory at all exit points to prevent memory leaks.

## 2025-02-13 - Plaintext Credential Logging in URI Handlers
**Vulnerability:** Raw HTTP POST body buffers containing sensitive parameters (like WPA2 identities, Wi-Fi passwords, or URL-encoded credentials) were logged using `ESP_LOGI(TAG, "getting content %s", buf);` in URI handlers like `portmaphandler.c`, `applyhandler.c`, and `otahandler.c`.
**Learning:** General-purpose logging of request payloads easily results in unintended plaintext exposure of sensitive data in application console output or logs.
**Prevention:** Do not log raw HTTP payloads if they can contain sensitive user information. Process request components first, redact sensitive values, or avoid logging parameters altogether if not explicitly needed for debugging.
## 2024-05-24 - Stop logging plaintext credential buffers in URI handlers
**Vulnerability:** Raw HTTP POST buffers (`buf`, `content`) were logged directly to the console via `ESP_LOGI` before parsing. This inadvertently exposed user-submitted credentials (like passwords and identity tokens) in plaintext.
**Learning:** General "getting content" debug logging is often added blindly without considering the sensitive nature of the accumulated data.
**Prevention:** Avoid logging raw buffers containing multiple form fields. Instead, rely on parameter-specific logging mechanisms (like `readUrlParameterIntoBuffer`) which can apply heuristic redaction to individual sensitive keys.
## 2025-02-13 - Stored XSS via Unsanitized appliedSSID
**Vulnerability:** The HTTP POST `ssid` parameter in `index_post_handler` (`src/urihandler/indexhandler.c`) was stored directly into the global `appliedSSID` string without sanitization. Later, in `index_get_handler`, this string was directly injected into the `config_page` HTML response via `sprintf()`, leading to a Stored Cross-Site Scripting (XSS) vulnerability.
**Learning:** Any user-supplied data obtained from URL parameters or request bodies that is later rendered into an HTML interface must be strictly entity-encoded to prevent malicious script execution in the client's browser.
**Prevention:** Implement a standard `sanitize_html` utility to escape HTML special characters (`<`, `>`, `&`, `"`, `'`) and apply it immediately when extracting strings that will be reflected back to the UI. Ensure bounds-checking during sanitization expansion.

## 2024-04-20 - Unprotected Administrative Endpoint (Reset Page)
**Vulnerability:** The `/reset` HTTP GET endpoint (`reset_get_handler` in `statichandler.c`) lacked the `isLocked()` authorization check, allowing any unauthenticated user on the local network to access the reset device page even when the router UI was locked with a password.
**Learning:** Administrative actions, even if they only present a confirmation page rather than performing the destructive action directly, must be protected behind the authentication lock screen. If the UI endpoint is unprotected, it breaks the intended security posture by exposing administrative functionality.
**Prevention:** All handlers returning administrative or sensitive UI HTML pages must start with an `isLocked()` check, matching the security applied to the backend POST endpoints.

## 2026-04-26 - Uninitialized Pointer Dereference with get_config_param_str
**Vulnerability:** The lock handler allocated memory via `get_config_param_str("lock_pass", &lock)`, but if the read failed, `lock` remained uninitialized. The code subsequently crashed when calling `strcmp(lock, unlockParam)` with a garbage pointer. Furthermore, it did not free the `lock` memory when passwords matched or failed.
**Learning:** Functions that conditionally allocate memory via pointer-to-pointer arguments leave variables uninitialized on error. Passing uninitialized or NULL pointers to `strcmp` causes a segmentation fault crash.
**Prevention:** Always initialize pointers passed to dynamic allocation functions to `NULL` (e.g., `char *lock = NULL;`). Always check `if (lock != NULL)` before using it in string functions, and ensure memory is `free()`d under all conditional branches to avoid leaks.
## 2026-05-18 - Plaintext Password Exposure in CLI Log Output
**Vulnerability:** The command-line router component (`components/cmd_router/cmd_router.c`) printed sensitive network and authentication credentials (like WPA Enterprise identities, STA passwords, and AP passwords) in plaintext to the UART console via `ESP_LOGI` when configurations were saved, and via `printf` when the `show` command was executed.
**Learning:** Hardcoded telemetry and informational logging often blindly print entire data structures without filtering sensitive fields, creating a CWE-532 vulnerability where credentials meant for flash memory are exposed in easily accessible logs.
**Prevention:** Explicitly redact sensitive fields (e.g., replace with `***REDACTED***`) when printing configuration objects to `ESP_LOGI` or `printf`. Avoid logging raw authentication material during setup or status commands.
## 2025-04-29 - Cross-Site Scripting (XSS) via Unsanitized Wi-Fi SSIDs
**Vulnerability:** A Stored XSS vulnerability in `src/urihandler/resulthandler.c` allowed malicious Wi-Fi Access Point names (SSIDs) to inject arbitrary HTML/JavaScript into the web UI when a user viewed the scan results page.
**Learning:** External environmental data, even from low-level network operations like an ESP32 Wi-Fi scan (`esp_wifi_sta_get_ap_info`), is untrusted user input. SSIDs are frequently used as attack vectors against IoT devices with web interfaces.
**Prevention:** Always allocate a safely sized buffer (e.g., `max_len * 6` to account for max HTML entity expansion) and run untrusted network string fields through `sanitize_html()` before rendering them into HTML templates using `snprintf()`.
