## 2024-06-25 - Prevent Memory Leaks and Fragmentation in Loops

**Learning:** Allocating dynamic memory (`malloc`) for temporary string operations inside a loop (like iterating through connected clients or WiFi scan results) without a corresponding `free()` causes massive memory leaks that degrade performance and crash the system over time. Even if `free()` is used, repetitive dynamic allocation can cause heap fragmentation on constrained devices.
**Action:** When building strings iteratively in loops, use stack-allocated buffers (e.g., `char buffer[SIZE]`) declared outside the loop whenever the size is small and predictable, or reuse a single dynamically allocated buffer to prevent both leaks and fragmentation.

## 2025-02-27 - O(N^2) String Concatenation in Loops

**Learning:** In C, using `strcat` to build a string dynamically inside a loop leads to an O(N^2) operation because `strcat` traverses the entire accumulated string buffer on each iteration to find the null terminator. On constrained devices like the ESP32, this can result in noticeable latency and wastes CPU cycles.
**Action:** When appending strings continuously in a loop, avoid `strcat`. Instead, track the current offset and write directly to the buffer using `snprintf(buffer + offset, sizeof(buffer) - offset, ...)`. Keep track of the added bytes to update the `offset`, ensuring O(N) linear time and preventing buffer overflows.

## 2025-02-28 - O(N^2) Latency in HTTP Chunk Accumulation

**Learning:** The firmware accumulates HTTP POST chunks by reading `httpd_req_recv` into a temporary chunk buffer, followed by `strcat` and `strlen` on the growing total payload buffer. This forces the device to copy intermediate chunks and rescan the entire accumulated string on every chunk, drastically increasing CPU cycles and processing time for larger requests.
**Action:** Read `httpd_req_recv` directly into the final pre-allocated target buffer at a tracked offset (e.g. `target_buffer + current_offset`), eliminating the intermediate chunk buffer, the O(N^2) `strcat`, and redundant `strlen` calls.
