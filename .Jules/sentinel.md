## 2024-07-22 - Fix Buffer Overflow via strcpy in otahandler

**Vulnerability:** Unbounded use of `strcpy` and `strcat` in `src/urihandler/otahandler.c` when writing custom URLs or predefined URLs into a fixed-size buffer. If an attacker controlled the URL parameter (e.g. through a maliciously crafted custom OTA update URL), it could lead to stack buffer overflow and potential application crash or RCE.
**Learning:** Helper functions that handle string copying should always explicitly receive the destination buffer's maximum size. Passing `sizeof(buf)` from the caller site is a reliable way to enforce bounds checking inside the helper function via `snprintf` or `strlcpy`.
**Prevention:** Always use `snprintf` or `strlcpy` in place of `strcpy` and `strcat`. When refactoring, ensure that helper functions receive the bounds explicitly via function parameters, rather than hardcoding or assuming the array size.
## 2024-08-05 - Fix Stack Buffer Overflow via strcat/strncpy in portmaphandler
**Vulnerability:** Unbounded use of `strcat` coupled with `strncpy` missing explicit null-termination logic on a Variable Length Array (VLA) in `src/urihandler/portmaphandler.c`. An attacker controlling the URL parameter could potentially write past the bounds of the array, leading to a stack buffer overflow.
**Learning:** When initializing a buffer with a prefix string using an offset (to drop the last character) followed by appending an untrusted suffix, using `strncpy` and `strcat` is highly prone to bounds-checking mistakes.
**Prevention:** Replace manual prefix slicing (`strncpy` + null termination) and unbounded suffix appending (`strcat`) with a single `snprintf` call that enforces the buffer boundary using `sizeof` and leverages string precision specifiers (e.g., `"%.*s%s"`) for clean slicing.
## 2024-08-20 - Fix Memory Leak in Early Return Path

**Vulnerability:** A memory leak occurs in `src/urihandler/indexhandler.c` (`index_get_handler`) when an early return is triggered by the `result_shown == 0` condition. The variables `sta_identity`, `sta_user`, and `cert`, which were dynamically allocated by `get_config_param_str_from_nvs` and `get_config_param_blob_from_nvs`, were not being freed before the early return, potentially leading to heap exhaustion (Denial of Service).
**Learning:** Early return paths in C HTTP handlers are a common source of memory leaks if dynamically allocated resources are not explicitly cleaned up before the return statement.
**Prevention:** Always trace the allocation lifecycle of variables in a function, ensuring all allocated pointers are properly freed on every execution branch, especially error or early-exit branches.
