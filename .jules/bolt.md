## 2025-05-14 - Optimized String Concatenation in Request Receiving
**Learning:** Using `strcat` in a loop for accumulating network data leads to O(N^2) time complexity, which is highly inefficient for large payloads on constrained devices.
**Action:** Replace `strcat` loops with offset-based writes (e.g., `memcpy` or direct buffer access with an offset) to achieve O(N) complexity.
