1. **Identify Vulnerability:** The `unlock_handler` function in `src/urihandler/lockhandler.c` contains a double-free vulnerability. The variable `unlockParam` can be freed twice:
   - Line 64: `free(unlockParam);` inside the `if (strlen(unlockParam) > 0)` block (if `lock != NULL` but passwords do not match).
   - Line 70: `free(unlockParam);` outside the block.
2. **Fix Vulnerability:** Remove the redundant `free(unlockParam)` at line 64.
3. **Verify Code Change:** Use `cat src/urihandler/lockhandler.c | grep -n "free("` to verify that `unlockParam` is no longer freed twice in `unlock_handler`.
4. **Compile Firmware:** Run `python3 -m platformio run -e esp32` to ensure the C code compiles correctly without syntax errors.
5. **Pre-commit Checks:** Call `pre_commit_instructions` and follow its instructions to complete pre-commit steps.
6. **Submit PR:** Submit the change with branch `sentinel-fix-double-free` and title `🛡️ Sentinel: [CRITICAL] Fix double-free in unlock handler`.
