## 2026-06-07 - Add textual loading indicators to waiting pages
**Learning:** Bootstrap CSS spinners (like `spinner-border`) are often purged in minified CSS files in this project to save space. Attempting to use them results in non-functional static states.
**Action:** Use standard text and unicode characters (e.g., `&#8635;` for restart) wrapped in `<span aria-hidden="true">` to create accessible and functional loading indicators without relying on external CSS.
