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
## 2025-02-28 - VLA Stack Buffers via Substring Calculations
**Vulnerability:** Calculating dynamic sizes using  to allocate Variable Length Arrays (VLAs) dynamically on the stack inside URI handlers caused stack overflow vulnerability risks for user-supplied string data. Additionally, missing bounds checks on string format operations could cause out-of-bounds writes.
**Learning:** Relying on  to allocate variables on constrained embedded task stacks is a critical design flaw. Furthermore, using unprotected / blindly is error-prone.
**Prevention:** Replace dynamically sized VLAs with fixed-length, conservatively sized buffers (e.g. `char resultIP[128]`), and format outputs exclusively using  combined with bounds checking.
## 2025-02-28 - VLA Stack Buffers via Substring Calculations
**Vulnerability:** Calculating dynamic sizes using `strlen` to allocate Variable Length Arrays (VLAs) dynamically on the stack inside URI handlers caused stack overflow vulnerability risks for user-supplied string data. Additionally, missing bounds checks on string format operations could cause out-of-bounds writes.
**Learning:** Relying on `strlen(user_input)` to allocate variables on constrained embedded task stacks is a critical design flaw. Furthermore, using unprotected `sprintf`/`strcat` blindly is error-prone.
**Prevention:** Replace dynamically sized VLAs with fixed-length, conservatively sized buffers (e.g. `char resultIP[128]`), and format outputs exclusively using `snprintf` combined with bounds checking.
