## 2024-06-03 - [Stored XSS via HTTPD Format Strings]
**Vulnerability:** Found Stored Cross-Site Scripting (XSS) where unescaped NVS string parameters (`ap_ssid`, `ap_passwd`, `ssid`, `passwd`, `sta_identity`, `sta_user`, `cer`) were injected directly into the HTML configuration page using `snprintf`.
**Learning:** While `appliedSSID` was explicitly sanitized, other variables read directly from configuration storage were assumed safe or missed, exposing a widespread XSS vector within the `index_get_handler`.
**Prevention:** Always create a centralized, heap-allocated sanitization helper (e.g., `alloc_safe_str`) and use it to escape ALL user-controlled configuration strings immediately before injecting them into HTML templates to prevent stored XSS and avoid stack overflows.
## 2024-06-04 - [Centralized XSS Mitigation]
**Vulnerability:** Stored XSS vectors (e.g., `ota_url` in `otahandler.c`) were left unmitigated because safe string allocation helpers (`alloc_safe_str`) were locally scoped to individual handlers (like `indexhandler.c`), discouraging reuse.
**Learning:** Relying on locally scoped HTML sanitization functions breeds inconsistent escaping and leaves other HTTP endpoints vulnerable when retrieving and formatting NVS parameters.
**Prevention:** Ensure heap-allocated HTML sanitization helpers (e.g., `alloc_safe_str`) are globally declared in shared headers (e.g., `helper.h`) and centrally implemented to standardize XSS mitigation across the entire codebase.
