## 2024-05-18 - [Apply page loading indicator]
**Learning:** UX states representing long operations (like apply.html rebooting) can feel stagnant or look like errors if they just display static text.
**Action:** Always include a visual loading indicator (like a spinner) along with descriptive text when displaying a meta refresh page representing a wait state.

## 2024-05-18 - [Missing CSS animations]
**Learning:** The project uses a purged/minified Bootstrap CSS file (`styles-*.css`). Unused classes like `spinner-border` were purged to save space. Adding them to HTML does not work because the CSS is missing.
**Action:** Do NOT manually edit or append to the minified CSS file to restore them, as this is an unmaintainable anti-pattern rejected in code review. Instead, restrict UX enhancements to standard classes already present, or verify presence in the purged CSS before relying on them.
