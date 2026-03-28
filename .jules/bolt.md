## 2024-06-25 - Prevent Memory Leaks and Fragmentation in Loops

**Learning:** Allocating dynamic memory (`malloc`) for temporary string operations inside a loop (like iterating through connected clients or WiFi scan results) without a corresponding `free()` causes massive memory leaks that degrade performance and crash the system over time. Even if `free()` is used, repetitive dynamic allocation can cause heap fragmentation on constrained devices.
**Action:** When building strings iteratively in loops, use stack-allocated buffers (e.g., `char buffer[SIZE]`) declared outside the loop whenever the size is small and predictable, or reuse a single dynamically allocated buffer to prevent both leaks and fragmentation.

## 2025-02-27 - O(N^2) String Concatenation in Loops

**Learning:** In C, using `strcat` to build a string dynamically inside a loop leads to an O(N^2) operation because `strcat` traverses the entire accumulated string buffer on each iteration to find the null terminator. On constrained devices like the ESP32, this can result in noticeable latency and wastes CPU cycles.
**Action:** When appending strings continuously in a loop, avoid `strcat`. Instead, track the current offset and write directly to the buffer using `snprintf(buffer + offset, sizeof(buffer) - offset, ...)`. Keep track of the added bytes to update the `offset`, ensuring O(N) linear time and preventing buffer overflows.

## 2025-02-28 - O(N^2) Chunked Request Reading

**Learning:** When receiving chunked HTTP requests, iteratively appending the chunks to an accumulating buffer using `strcat` results in $O(N^2)$ time complexity and redundant memory copying.
**Action:** When reading HTTP request data into an intermediate buffer to process it later, use helper functions (e.g., `fill_post_buffer`) which reads directly into the final buffer at a tracked offset and sets the terminating null byte, ensuring $O(N)$ overall time complexity.

## 2025-03-01 - Prevent Heap Corruption and Minimize Malloc Overhead

**Learning:** Allocating memory dynamically via `malloc` for small strings, then conditionally freeing a pointer that might still reference a string literal (e.g., `char *ptr = ""; ... if (cond) ptr = malloc(16); ... free(ptr);`), leads to undefined behavior and fatal crashes when the condition is false. Additionally, repeated dynamic allocation of small fixed-size strings wastes cycles and increases heap fragmentation on constrained devices.
**Action:** For small, predictable string allocations (like an IPv4 address string which requires at most 16 bytes), utilize a fixed-size stack-allocated Variable Length Array (e.g., `char ptr[16] = "";`) instead of `char *ptr` and `malloc`. This prevents heap corruption when `free` is called, avoids `malloc` overhead entirely, and eliminates the risk of memory leaks.
