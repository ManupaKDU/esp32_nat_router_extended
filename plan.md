1. **Optimize `HTTPD_RESP_USE_STRLEN` in Handlers:**
   - Modify `resulthandler.c` to capture the length returned by `snprintf` and pass it to `httpd_resp_send` instead of `HTTPD_RESP_USE_STRLEN`. This eliminates two implicit, costly `strlen` calls on a potentially large (up to ~18KB) dynamically allocated HTML string.
   - Do the same in `advancedhandler.c` and `clientshandler.c`.
2. **Verify changes:**
   - Compile code with `python3 -m platformio run -e esp32` to ensure there are no syntax errors.
   - Run host tests `bash tests/run_tests.sh`.
3. **Pre Commit Steps:**
   - Complete pre-commit steps to make sure proper testing, verifications, reviews and reflections are done.
4. **Submit PR:**
   - Create a PR using the requested title format "⚡ Bolt: [performance improvement]".
