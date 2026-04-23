## 2024-05-18 - [Add Keyboard Support to Password Toggles]
**Learning:** [jQuery touch/mouse event listeners on static HTML elements (`span.password`) often fail screen reader and keyboard accessibility checks unless explicitly paired with `keydown`/`keyup` events and proper `role="button"` attributes.]
**Action:** [When identifying custom interactive span/div elements acting as buttons, ensure they have `tabindex="0"`, `role="button"`, `aria-label`, and explicitly handle 'Enter'/'Space' keys via JS event listeners.]

## 2024-03-18 - [Remove maxlength from number inputs]
**Learning:** [`maxlength` attribute is not supported and conflicts with `<input type="number">`. It behaves unpredictably across browsers and disrupts built-in numerical validation or mobile keypad presentation.]
**Action:** [Always use `min` and `max` attributes for number limits instead of `maxlength` when using numerical inputs.]
## 2025-03-18 - Neutral Empty States & HTML5 Validation
**Learning:** Empty states using error colors (`text-danger`) cause unnecessary alarm, while HTML5 `required` attributes provide instant, accessible inline validation without complex JS.
**Action:** When creating empty states, use neutral styling like `text-muted`. For simple form validation, leverage native HTML5 attributes before implementing custom scripts.

## 2024-05-18 - [Add JS Confirmations for Destructive Actions in Table Rows]
**Learning:** [Forms generating inline destructive actions (like row deletions) often lack standard UI confirmations, leading to accidental clicks, especially on mobile where touch targets are small. Additionally, interactive UI elements generated via C templates can easily miss closing tags (like `</button>`), which breaks HTML structure and screen reader accessibility.]
**Action:** [Always include an `onsubmit="return confirm('...');"` on forms performing destructive inline actions to prevent accidental data loss, and double check C-string templates for properly closed semantic tags.]

## 2025-03-26 - Add inline validation and accessibility labels to advanced network inputs
**Learning:** For embedded interfaces where rendering and validation are handled by an underlying C-backend (like ESP32 firmware), native HTML5 inline validation attributes (`pattern`, `title`, `required`) are highly effective. They enforce data constraints (like IPv4 or MAC address formats) on the client side without relying on complex JS or server roundtrips, providing immediate feedback and preventing malformed data from reaching the backend. Additionally, inputs that visually relate to a preceding radio button (like "Custom" options) often lack explicit programmatic association, necessitating `aria-label`s for screen reader users.
**Action:** When working on C-backed web interfaces, prefer native HTML5 validation attributes over custom JS validation to keep the frontend lightweight. Always verify that text inputs tied to radio button selections have explicit accessible names (`aria-label`) if the visual `<label>` is bound to the radio button.
## 2024-04-19 - Destructive Form Validation
**Learning:** In the ESP32 NAT Router UI, some destructive actions like erasing flash memory are triggered via dedicated POST forms instead of asynchronous JavaScript calls.
**Action:** When adding confirmation dialogs to these forms to prevent accidental data loss, apply `onsubmit="return confirm('...');"` directly to the HTML `<form>` element rather than attaching event listeners to the submit button to ensure it intercepts standard form submission behavior cleanly.

## 2024-04-23 - Native Cross-Field Form Validation
**Learning:** For cross-field form validation (like matching passwords) in pages without external JavaScript libraries, utilizing `oninput` handlers with `setCustomValidity` natively integrates with the browser's form validation API. This allows screen readers to announce validation errors and leverages native UI without writing additional inline script tags.
**Action:** When adding cross-field constraints on simpler HTML templates, apply `oninput="document.getElementById('otherId').setCustomValidity(...)"` to enforce validation declaratively and accessibly.
