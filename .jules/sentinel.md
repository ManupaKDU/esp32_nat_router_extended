## 2024-06-03 - [Stored XSS via HTTPD Format Strings]
**Vulnerability:** Found Stored Cross-Site Scripting (XSS) where unescaped NVS string parameters (`ap_ssid`, `ap_passwd`, `ssid`, `passwd`, `sta_identity`, `sta_user`, `cer`) were injected directly into the HTML configuration page using `snprintf`.
**Learning:** While `appliedSSID` was explicitly sanitized, other variables read directly from configuration storage were assumed safe or missed, exposing a widespread XSS vector within the `index_get_handler`.
**Prevention:** Always create a centralized, heap-allocated sanitization helper (e.g., `alloc_safe_str`) and use it to escape ALL user-controlled configuration strings immediately before injecting them into HTML templates to prevent stored XSS and avoid stack overflows.
