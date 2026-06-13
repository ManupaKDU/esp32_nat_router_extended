1. **Fix missing `free()` calls in HTTP handlers for dynamically allocated configuration values.**
    - `src/http_server.c`: Add `free(lock_pass)` after checking it in `start_webserver`.
    - `src/urihandler/indexhandler.c`: Add `free(lock_pass)` after checking it in `index_get_handler`. Add `free(sta_identity)` and `free(sta_user)` at the end of `index_get_handler`. Add `free(result_param)` if it is not null in `index_get_handler`.
    - `src/urihandler/lockhandler.c`: Add `free(lock_pass)` after using it in `lock_handler`.
    - `src/urihandler/resulthandler.c`: Add `free(result_param)` after using it in `result_download_get_handler`.
    - `src/urihandler/otahandler.c`: Add `free(customUrl)` in `getOtaUrl` after copying it.
    - `src/esp32_nat_router.c`: Add `free(scan_result)` after checking it in `app_main`. (Also double check if other things like `customDNS`, `hostName`, `customMac`, `loglevel` read in other `fill`/`set` functions leak memory there. It looks like they might.)

2. **Verify changes by running tests.**
    - Use `./tests/run_tests.sh` to ensure no functionality is broken.

3. **Complete pre-commit steps to ensure proper testing, verification, review, and reflection are done.**
    - Use the `pre_commit_instructions` tool.

4. **Submit a PR.**
    - Create a branch and PR with a descriptive title format '🛡️ Sentinel: [MEDIUM] Fix memory leaks in configuration retrieval'.
