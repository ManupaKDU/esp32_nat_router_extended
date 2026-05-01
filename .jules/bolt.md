## 2024-05-14 - [O(n^2) String Concatenation Bottleneck]
**Learning:** Using `strcat` inside a loop requires scanning the entire destination string to find the NUL terminator on every iteration, resulting in O(n^2) performance. Furthermore, performing small, repeated memory allocations (`malloc`/`free`) inside the loop creates unnecessary heap fragmentation and latency.
**Action:** When building strings iteratively, use the return value of `sprintf`/`snprintf` to advance a pointer directly to the end of the buffer (`ptr += len`), avoiding both `strcat` and redundant memory allocations.
## 2024-05-15 - [Constant Time Compile-time String Lengths]
**Learning:** Using `strlen()` on string literals (like `"checked"`, `"selected"`, `"http://"`) calculates the length at runtime during every HTTP request execution. This incurs unnecessary CPU cycles, especially when repeated across many string literals in dynamic HTML page generation.
**Action:** Replace `strlen("literal")` with `(sizeof("literal") - 1)` for string constants to compute the string length at compile-time instead of runtime.
