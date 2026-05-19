## 2024-05-19 - Added loading spinner to wait pages
**Learning:** When using purged CSS files (like `styles-67aa3b0203355627b525be2ea57be7bf.css`), classes not used in the original HTML templates are missing. Adding new elements like loading spinners requires appending the raw minified CSS definitions for `.spinner-border` and `.visually-hidden`.
**Action:** Always verify if new Bootstrap classes exist in the minified CSS and hardcode them at the end of the CSS file if missing.
