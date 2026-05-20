## 2024-05-20 - Purged CSS in Minified Environments
**Learning:** This application purges unused Bootstrap CSS classes to save space on the ESP32 (e.g. `spinner-border` was removed). Modifying the minified CSS manually to restore them is unmaintainable and rejected in code review.
**Action:** When adding UX enhancements, restrict usage to standard classes already in use (e.g., swapping `btn-warning` to `btn-danger` for consistency), or verify presence in the purged CSS before relying on new classes.
