## 2026-05-18 - Accessible Spinners and Purged CSS
**Learning:** When adding new UI elements (like a loading spinner) to static HTML pages, the required Bootstrap classes (like `.spinner-border` and `.visually-hidden`) may be missing from the purged minified CSS file.
**Action:** Always manually verify the existence of CSS classes in the minified stylesheet and append them manually using a simple python script if necessary to ensure correct rendering and accessibility.
