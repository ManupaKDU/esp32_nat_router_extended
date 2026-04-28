## 2024-05-18 - [Add JS Confirmations for Destructive Actions in Table Rows]
**Learning:** [Forms generating inline destructive actions (like row deletions) often lack standard UI confirmations, leading to accidental clicks, especially on mobile where touch targets are small. Additionally, interactive UI elements generated via C templates can easily miss closing tags (like `</button>`), which breaks HTML structure and screen reader accessibility.]
**Action:** [Always include an `onsubmit="return confirm('...');"` on forms performing destructive inline actions to prevent accidental data loss, and double check C-string templates for properly closed semantic tags.]

## 2024-04-19 - Destructive Form Validation
**Learning:** In the ESP32 NAT Router UI, some destructive actions like erasing flash memory or removing passwords are triggered via dedicated POST forms instead of asynchronous JavaScript calls.
**Action:** When adding confirmation dialogs to these forms to prevent accidental data loss, apply `onsubmit="return confirm('...');"` directly to the HTML `<form>` element rather than attaching event listeners to the submit button to ensure it intercepts standard form submission behavior cleanly.
