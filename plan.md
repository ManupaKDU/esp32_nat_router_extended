1. Modify `src/pages/config.html` using `replace_with_git_merge_diff` to add `aria-controls="wpa2-container"` to the `#wpa2enabled` checkbox.
2. Verify `src/pages/config.html` changes using `run_in_bash_session` with `grep`.
3. Modify `src/pages/config.html` using `replace_with_git_merge_diff` to update the JavaScript. The new script will sync `aria-expanded`, disable/enable all form elements (`input, textarea, select`), and trigger on `$(document).ready()`.
4. Verify the JavaScript changes in `src/pages/config.html` using `run_in_bash_session` with `grep`.
5. Append a new entry to `.jules/palette.md` about dynamically hiding/showing form sections and accessibility synchronization using `run_in_bash_session` and `cat << 'EOF' >>`.
6. Verify `.jules/palette.md` using `run_in_bash_session` with `cat`.
7. Call `frontend_verification_instructions` to get steps for frontend verification.
8. Run the full application build using `run_in_bash_session` with `pio run -e esp32` to ensure no regressions.
9. Run the test suite using `run_in_bash_session` with `bash tests/run_tests.sh`.
10. Execute the frontend verification using `run_in_bash_session` to run a Python Playwright script that navigates to the config page, toggles the WPA2 option, captures a screenshot, and records a video, and then call `frontend_verification_complete`.
11. Clean up background server and verification files using `run_in_bash_session`.
12. Call `pre_commit_instructions` to get pre-commit steps.
13. Complete pre-commit steps to ensure proper testing, verification, review, and reflection are done using `run_in_bash_session`.
14. Call `submit` to create the PR with the required Palette persona format.
