## 2024-07-22 - Fix Buffer Overflow via strcpy in otahandler

**Vulnerability:** Unbounded use of `strcpy` and `strcat` in `src/urihandler/otahandler.c` when writing custom URLs or predefined URLs into a fixed-size buffer. If an attacker controlled the URL parameter (e.g. through a maliciously crafted custom OTA update URL), it could lead to stack buffer overflow and potential application crash or RCE.
**Learning:** Helper functions that handle string copying should always explicitly receive the destination buffer's maximum size. Passing `sizeof(buf)` from the caller site is a reliable way to enforce bounds checking inside the helper function via `snprintf` or `strlcpy`.
**Prevention:** Always use `snprintf` or `strlcpy` in place of `strcpy` and `strcat`. When refactoring, ensure that helper functions receive the bounds explicitly via function parameters, rather than hardcoding or assuming the array size.
