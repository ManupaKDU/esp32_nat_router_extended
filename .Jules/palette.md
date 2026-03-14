## 2024-05-18 - [Add Keyboard Support to Password Toggles]
**Learning:** [jQuery touch/mouse event listeners on static HTML elements (`span.password`) often fail screen reader and keyboard accessibility checks unless explicitly paired with `keydown`/`keyup` events and proper `role="button"` attributes.]
**Action:** [When identifying custom interactive span/div elements acting as buttons, ensure they have `tabindex="0"`, `role="button"`, `aria-label`, and explicitly handle 'Enter'/'Space' keys via JS event listeners.]
