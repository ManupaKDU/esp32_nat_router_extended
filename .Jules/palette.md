## 2024-05-18 - [Add Keyboard Support to Password Toggles]
**Learning:** [jQuery touch/mouse event listeners on static HTML elements (`span.password`) often fail screen reader and keyboard accessibility checks unless explicitly paired with `keydown`/`keyup` events and proper `role="button"` attributes.]
**Action:** [When identifying custom interactive span/div elements acting as buttons, ensure they have `tabindex="0"`, `role="button"`, `aria-label`, and explicitly handle 'Enter'/'Space' keys via JS event listeners.]

## 2024-03-18 - [Remove maxlength from number inputs]
**Learning:** [`maxlength` attribute is not supported and conflicts with `<input type="number">`. It behaves unpredictably across browsers and disrupts built-in numerical validation or mobile keypad presentation.]
**Action:** [Always use `min` and `max` attributes for number limits instead of `maxlength` when using numerical inputs.]
## 2025-03-18 - Neutral Empty States & HTML5 Validation
**Learning:** Empty states using error colors (`text-danger`) cause unnecessary alarm, while HTML5 `required` attributes provide instant, accessible inline validation without complex JS.
**Action:** When creating empty states, use neutral styling like `text-muted`. For simple form validation, leverage native HTML5 attributes before implementing custom scripts.
