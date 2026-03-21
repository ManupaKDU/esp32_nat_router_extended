## 2024-05-18 - [Add Keyboard Support to Password Toggles]
**Learning:** [jQuery touch/mouse event listeners on static HTML elements (`span.password`) often fail screen reader and keyboard accessibility checks unless explicitly paired with `keydown`/`keyup` events and proper `role="button"` attributes.]
**Action:** [When identifying custom interactive span/div elements acting as buttons, ensure they have `tabindex="0"`, `role="button"`, `aria-label`, and explicitly handle 'Enter'/'Space' keys via JS event listeners.]

## 2024-03-18 - [Remove maxlength from number inputs]
**Learning:** [`maxlength` attribute is not supported and conflicts with `<input type="number">`. It behaves unpredictably across browsers and disrupts built-in numerical validation or mobile keypad presentation.]
**Action:** [Always use `min` and `max` attributes for number limits instead of `maxlength` when using numerical inputs.]

## 2024-03-20 - [Hide decorative SVGs from screen readers]
**Learning:** [Screen readers may redundantly announce icon fonts or SVG symbols if they are enclosed in buttons or links that already possess an `aria-label` or surrounding textual context.]
**Action:** [Always add `aria-hidden="true"` to decorative `<svg>` elements inside interactive components to prevent screen readers from reading extraneous descriptions.]

## 2024-03-20 - [Confirmation for destructive actions]
**Learning:** [Destructive actions inside embedded C strings (e.g. `PORTMAP_ROW_TEMPLATE` in `portmaphandler.c`) often lack confirmation prompts, leading to accidental deletions by users.]
**Action:** [Implement a basic JavaScript confirmation dialog (`onsubmit="return confirm('...');"`) on forms that execute destructive actions (like deleting items) to prevent accidental data loss.]
