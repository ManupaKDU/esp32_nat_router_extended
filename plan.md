1. **Understand the problem**:
   The user wants a micro-UX improvement that adds delight and accessibility to the UI.
   We have identified that pages involving long-running device operations, such as `apply.html` (rebooting) and `scan.html` (network scanning), utilize `<meta http-equiv="refresh">` tags to simulate waiting states before redirecting.
   When enhancing these pages, adding an accessible visual loading indicator (like a spinner with `role="status"` and `visually-hidden` text) would be a great UX improvement to give the user immediate visual feedback that the application is busy.

2. **Verify missing CSS classes**:
   Since the project uses a purged/minified Bootstrap CSS file `styles-*.css`, the classes `.d-flex`, `.justify-content-center`, `.spinner-border`, `@keyframes spinner-border`, and `.visually-hidden` might be missing if they weren't used in other HTML files.
   We need to append them manually to the minified CSS.

   ```css
   .d-flex{display:flex!important}.justify-content-center{justify-content:center!important}.spinner-border{display:inline-block;width:2rem;height:2rem;vertical-align:-.125em;border:.25em solid currentColor;border-right-color:transparent;border-radius:50%;animation:.75s linear infinite spinner-border}@keyframes spinner-border{to{transform:rotate(360deg)}}.visually-hidden{position:absolute!important;width:1px!important;height:1px!important;padding:0!important;margin:-1px!important;overflow:hidden!important;clip:rect(0,0,0,0)!important;white-space:nowrap!important;border:0!important}
   ```

3. **Modify `apply.html` and `scan.html`**:
   Add the loading spinner markup to the waiting pages.

   In `apply.html`:
   ```html
        <div class="alert alert-warning mt-2 text-center" role=alert>
            <h2>The new settings have been applied and the device will be restarted.<br>Wait until the config page is
                reloaded...</h2>
            <div class="d-flex justify-content-center mt-4">
                <div class="spinner-border text-warning" role="status">
                    <span class="visually-hidden">Loading...</span>
                </div>
            </div>
        </div>
   ```

   In `scan.html`:
   ```html
        <div class="alert alert-light mt-2 text-center" role=alert>
            <h2 class="mt-2 text-center">The device will now be restarted and you will be disconnected. After 20s you
                will be forwarded to the result page. Make sure your device will automatically reconnect to the new
                network. Otherwise reconnect manually and visit <a href=http://%s>the default url</a></h2>
            <div class="d-flex justify-content-center mt-4">
                <div class="spinner-border text-secondary" role="status">
                    <span class="visually-hidden">Loading...</span>
                </div>
            </div>
        </div>
   ```

4. **Verify changes locally**:
   - Verify the modified HTML in the browser.
   - We can write a quick python test to serve the pages statically and use Playwright to take screenshots of the change if needed to verify it looks good.
   - Run compilation to ensure no syntax errors.

5. **Commit the changes**:
   - `git commit -m "🎨 Palette: Add loading spinners to waiting pages"`
