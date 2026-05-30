1. **Refactor `advancedhandler.c`**:
   - Use `replace_with_git_merge_diff` to capture the return value of `sprintf(advanced_page, advanced_start, hostName, octet, lowSelected, mediumSelected, highSelected, bwHigh, bwLow, ledCB, aliveCB, natCB, currentDNS, defCB, cloudCB, adguardCB, customCB, customDNSIP, currentMAC, defMacCB, defaultMAC, rndMacCB, subMac, customMacCB, customMac, netmask, classCCB, octet, classBCB, octet, classACB, octet, customMaskCB, customMask)` into an integer `response_len`.
   - Replace `HTTPD_RESP_USE_STRLEN` in `httpd_resp_send(req, advanced_page, HTTPD_RESP_USE_STRLEN)` with `response_len`.

2. **Verify changes to `advancedhandler.c`**:
   - Use `run_in_bash_session` with `grep` to verify the modified file was updated correctly.

3. **Refactor `resulthandler.c`**:
   - Use `replace_with_git_merge_diff` to capture the return value of `sprintf(result_page, result_start, result)` into an integer `response_len`.
   - Replace `HTTPD_RESP_USE_STRLEN` in `httpd_resp_send(req, result_page, HTTPD_RESP_USE_STRLEN)` with `response_len`.
   - Also, replace `strlen(result_page)` in the `ESP_LOGI` call with `response_len` to avoid another `strlen` call.

4. **Verify changes to `resulthandler.c`**:
   - Use `run_in_bash_session` with `grep` to verify the modified file was updated correctly.

5. **Run tests**:
   - Use `run_in_bash_session` to run tests using `bash tests/run_tests.sh`.

6. **Install dependencies**:
   - Use `run_in_bash_session` to install the required platformio version: `pip install "setuptools<70.0.0" platformio`.

7. **Compile firmware**:
   - Use `run_in_bash_session` to run a full firmware compilation with `python3 -m platformio run -e esp32`.

8. **Pre-commit steps**:
   - Complete pre-commit steps to ensure proper testing, verification, review, and reflection are done.

9. **Submit**:
   - Create PR using `submit` with the appropriate title format: `⚡ Bolt: [performance improvement]`.
