## 2024-06-25 - Prevent Memory Leaks and Fragmentation in Loops

**Learning:** Allocating dynamic memory (`malloc`) for temporary string operations inside a loop (like iterating through connected clients or WiFi scan results) without a corresponding `free()` causes massive memory leaks that degrade performance and crash the system over time. Even if `free()` is used, repetitive dynamic allocation can cause heap fragmentation on constrained devices.
**Action:** When building strings iteratively in loops, use stack-allocated buffers (e.g., `char buffer[SIZE]`) declared outside the loop whenever the size is small and predictable, or reuse a single dynamically allocated buffer to prevent both leaks and fragmentation.

## 2025-02-27 - O(N^2) String Concatenation in Loops

**Learning:** In C, using `strcat` to build a string dynamically inside a loop leads to an O(N^2) operation because `strcat` traverses the entire accumulated string buffer on each iteration to find the null terminator. On constrained devices like the ESP32, this can result in noticeable latency and wastes CPU cycles.
**Action:** When appending strings continuously in a loop, avoid `strcat`. Instead, track the current offset and write directly to the buffer using `snprintf(buffer + offset, sizeof(buffer) - offset, ...)`. Keep track of the added bytes to update the `offset`, ensuring O(N) linear time and preventing buffer overflows.

## 2025-02-28 - O(N^2) Chunked Request Reading

**Learning:** When receiving chunked HTTP requests, iteratively appending the chunks to an accumulating buffer using `strcat` results in $O(N^2)$ time complexity and redundant memory copying.
**Action:** When reading HTTP request data into an intermediate buffer to process it later, use helper functions (e.g., `fill_post_buffer`) which reads directly into the final buffer at a tracked offset and sets the terminating null byte, ensuring $O(N)$ overall time complexity.

## 2025-02-28 - NVS Portmap Table Atomicity and Wear

**Learning:** When updating table arrays backed by Non-Volatile Storage (NVS) (like `portmap_tab`), doing the save or side-effect operations (`save_portmap_tab()`, `ip_portmap_add()`) directly inside the discovery loop can lead to suboptimal code structure and potential wear or atomicity issues if the loop continues or is modified later, even if there is a `return` statement.
**Action:** Always follow a 'find then act' pattern for table updates: use a loop only to identify a target index (`target_idx`), then perform the NVS save and subsequent operations outside the loop to explicitly prevent flash wear, ensure atomicity, and maintain clean separation of concerns.

## 2025-02-28 - Optimizing Wi-Fi Station Count Checks

**Learning:** Calling `esp_wifi_ap_get_sta_list()` inside a tight loop or frequently across the codebase (like in the LED blinking thread and HTTP endpoints) introduces significant performance overhead, as it triggers internal core operations to fetch and copy MAC/RSSI details for all connected devices.
**Action:** Instead of proactively fetching the station list just to check the connection count, cache the active station count globally (`volatile uint16_t current_connect_count`) and update it incrementally using the native ESP Wi-Fi event handlers (`WIFI_EVENT_AP_STACONNECTED` and `WIFI_EVENT_AP_STADISCONNECTED`). This turns an O(N) hardware query into an O(1) memory read, saving CPU cycles.

## 2025-02-28 - Unnecessary Hardware Queries in Empty States

**Learning:** Unconditionally calling expensive ESP-IDF core functions like `esp_wifi_ap_get_sta_list()` when generating UI views (like the connected clients page) introduces significant latency and overhead, even when the station list is completely empty.
**Action:** When gathering data for a repeating UI element or list, always use O(1) state checks (like a globally cached `getConnectCount()`) as a guard clause *before* allocating memory and invoking the O(N) hardware API to fetch the actual details.

## 2024-04-17 - Optimize NVS configuration parameter read
**Learning:** Redundant calls to `get_config_param_str()` for the same key within a function (e.g., retrieving `custom_dns` multiple times) unnecessarily increase NVS flash read operations and introduce heap `malloc` overhead, negatively impacting performance and increasing fragmentation.
**Action:** Always reuse the initial allocated pointer or create a pointer alias (e.g., `customDNSIP = customDNS;`) to avoid redundant hardware queries and memory allocations. Ensure the primary pointers are properly `free()`d at the end of the HTTP handler to prevent memory leaks.

## 2026-04-11 - Prevent Redundant NVS Reads
**Learning:** Re-fetching the same configuration key from NVS via `get_config_param_str()` in conditionals adds unnecessary flash wear, read latency, and dynamic allocation overhead.
**Action:** Assign existing string pointers to subsequent variables if the key and value are identical, saving an NVS read and memory allocation overhead.

## 2026-04-11 - Memory Management of Mixed Allocation Types
**Learning:** The `getNetmask()` function returns either a dynamically allocated string (from NVS) or a static literal (`DEFAULT_NETMASK_CLASS_C`). Blindly calling `free()` causes undefined behavior, while skipping `free()` entirely causes a memory leak.
**Action:** Use pointer comparison (`if (ptr != CONSTANT)`) rather than string comparison (`strcmp`) to safely determine if the returned string requires `free()`, covering edge cases where the dynamically allocated NVS value matches the constant string value.
