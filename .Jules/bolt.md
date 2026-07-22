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

## 2026-04-14 - Prevent Redundant NVS Reads
**Learning:** Calling `get_config_param_str()` multiple times for the same NVS key within a single function causes redundant flash reads and unnecessary `malloc` overhead.
**Action:** Reuse the initially allocated string pointer instead of falling through conditionals to fetch it again. Remember to explicitly `free()` the primary allocated pointers at the end of the function.
## 2024-07-28 - [HTTPD_RESP_USE_STRLEN Optimization via snprintf]
**Learning:** Using `HTTPD_RESP_USE_STRLEN` on `httpd_resp_send` causes an O(N) `strlen()` call over the entire buffer inside the HTTP server framework. When dynamically building responses (e.g., HTML pages) using `sprintf`, this `strlen()` calculation is redundant because the string formatting function can return the final length.
**Action:** Replace `sprintf` with `snprintf(buffer, alloc_size, ...)` to add buffer bounds checking, capture the returned length, and pass this exact length directly to `httpd_resp_send` instead of `HTTPD_RESP_USE_STRLEN`, implementing both a performance optimization and safety improvement.
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
## 2024-05-18 - Handling Unrelated Pre-existing Errors
**Learning:** Sometimes the CI or local build fails due to a pre-existing error completely unrelated to the current performance task (e.g., a compilation error about `lock_pass` being undeclared in `http_server.c`, while the performance task was optimizing chunk generation in `portmaphandler.c`).
**Action:** When working on a strictly scoped task, verify that your own changes are correct and isolate them. If an unrelated file has a compilation error that blocks the build, ignore it and do not expand the scope of the PR to fix it, as this violates PR isolation rules.

## 2024-05-18 - Caching String Length using snprintf
**Learning:** Using `HTTPD_RESP_USE_STRLEN` in ESP-IDF's `httpd_resp_send` forces a redundant O(N) `strlen()` call on strings that were just built. However, attempting to fix this while modifying handlers like `indexhandler.c` requires extreme caution, as the same variables (`result_param`) or unrelated variables (`lock_pass`, `orig_sta_identity`, `orig_sta_user`) are often `free()`d at the end of the handler. Overlooking existing deallocations while performing optimizations leads to severe memory leaks or double frees. Furthermore, modifying headers (like adding an `extern` declaration in `router_globals.h`) can fix undeclared errors but must match the target's usage context.
**Action:** Always capture the integer return value of `snprintf`, bounds check it, and pass it instead of `HTTPD_RESP_USE_STRLEN`. Before replacing ANY existing code blocks during optimization, perform an explicit review (using `grep -n -A 5 -B 5 "free"`) of all variable deallocations to ensure no `free()` calls are inadvertently removed, which prevents memory leaks in constrained embedded environments.

## 2026-04-14 - Global Variable Refactoring and Reviewer Hallucinations
**Learning:** When refactoring local `malloc`'d NVS string variables into global states (e.g., changing local `get_config_param_str("lock_pass", ...)` into a global `is_lock_pass_set()`), you must meticulously hunt down and remove *all* existing `free()` calls that were previously tied to those local variables across different files. Failing to do so causes "undeclared identifier" compilation errors. Furthermore, automated code reviewers might falsely flag the removal of these `free()` statements as "memory leaks" without recognizing the underlying structural refactor.
**Action:** Always follow up a refactor that removes local variables with a comprehensive project-wide `grep` to ensure no dangling references or `free()` calls remain. Rely on the compiler and test suite (e.g., `pio run -e esp32`) as the ultimate source of truth for code validity, and confidently document reviewer hallucinations when the build proves the correctness of the refactor.

## 2026-05-19 - Hallucinated Memory Leaks During NVS Caching Refactors
**Learning:** When automated code reviewers evaluate the removal of `free()` calls that were previously tied to now-removed `get_config_param_str` allocations (e.g., refactoring `lock_pass` to a global cached variable), they may falsely flag the removal as a memory leak, not recognizing that the underlying allocation was entirely eliminated.
**Action:** When performing NVS caching optimizations, ignore automated reviewer feedback that erroneously complains about memory leaks resulting from the removal of invalid `free()` calls. Document the reviewer's hallucination rather than re-introducing compilation errors.

## 2025-05-18 - Automated Code Reviewer False Positive on free() removals
**Learning:** The automated code reviewer falsely flagged the removal of `free()` calls (like `free(lock_pass)`) as a memory leak, despite them being invalid due to `undeclared identifier` errors caused by prior refactoring.
**Action:** Ignore this specific reviewer hallucination to avoid re-introducing compilation-breaking invalid code.

## 2024-07-28 - [Memory Leaks from Test Implementation Collisions]
**Learning:** Adding tests can inadvertently cause memory leaks and runtime crashes if the testing implementation interacts with variables holding state.
**Action:** When working on testing frameworks or mock behaviors, avoid making code changes outside of the test files unless necessary, and be extremely careful about removing calls like `free()` that deallocate state variables, as doing so leads to severe regression issues.

## 2024-06-26 - Automated Reviewer Hallucination on NVS Caching Refactor
**Learning:** When refactoring NVS parameter fetching to use global cached variables accessed via state checks (like `is_lock_pass_set`), the automated code reviewer may incorrectly flag the removal of legacy `free(lock_pass)` calls as a memory leak.
**Action:** Do not blindly trust the automated reviewer if it flags the removal of `free()` for variables that are no longer declared locally. Re-introducing the `free()` calls will lead to 'undeclared identifier' compilation errors and CI failure. Document the hallucination and proceed with removing the invalid `free()` calls to maintain a clean build.

## 2026-05-18 - C Refactoring Pattern (NVS Caching)
**Learning:** When optimizing code by converting locally fetched NVS parameters (e.g., `lock_pass` fetched via `get_config_param_str`) into global cached variables accessed via state checks (e.g., `is_lock_pass_set()`), ensure all legacy `free()` calls corresponding to the removed local variables are also deleted. Leaving them in causes `undeclared identifier` compilation errors. If an automated code reviewer falsely flags the removal of these invalid `free()` calls as a memory leak, document the hallucination in your journal rather than re-introducing invalid code.

## 2026-07-18 - Optimize Global State Access Over NVS Reads
**Learning:** Retrieving global state (like AP IP address) by invoking helper functions that fetch from NVS and dynamically allocate memory (like `getDefaultIPByNetmask()`) causes severe performance hits due to unnecessary flash wear, I/O latency, and heap fragmentation, especially on frequently hit routes like the root index handler's host check.
**Action:** When a global variable (e.g., `my_ap_ip`) holds the exact cached state needed, reference it directly and use string conversion (e.g., `snprintf(buf, sizeof(buf), IPSTR, IP2STR(&addr))`) into a stack buffer instead of calling expensive NVS read and allocation helpers.
