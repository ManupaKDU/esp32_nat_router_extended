## 2023-10-25 - [Empty States and Inline Validation]
**Learning:** Found empty state messages in tables styled with `text-danger` and `text-warning`, making normal zero-state conditions appear as system errors. Missing `required` attributes on add-item forms allowed invalid partial submissions.
**Action:** When creating empty states, use neutral styling (like `text-muted`) to avoid causing user alarm. Always leverage HTML5 `required` attributes for simple inline validation before resorting to backend handling.

## 2025-03-27 - [Contextual ARIA Labels for Data Table Buttons]
**Learning:** Found data tables (like Wi-Fi scan results) using identical, generic button labels (e.g., "Use") across rows. This creates accessibility issues for screen reader users who hear repeating generic text without context when tabbing through interactive elements.
**Action:** When creating action buttons within repeating data table rows, always provide an `aria-label` that includes contextual data (like the row's primary identifier, e.g., `aria-label='Use MyWiFi'`) to ensure screen reader clarity.

## 2025-05-15 - [Screen Reader Noise from Decorative SVGs]
**Learning:** Found several decorative `<svg>` icons (e.g., Bootstrap icons for adding, removing, or toggling password visibility) embedded inside interactive elements (like `<button>` or `<span>`) that already possessed an `aria-label` or descriptive text. This causes redundant and sometimes confusing noise for screen reader users, as the screen reader may attempt to announce the SVG structure or content alongside the button's intended label.
**Action:** When embedding decorative `<svg>` icons inside interactive elements that already have an `aria-label` or descriptive text, always add `aria-hidden="true"` to the `<svg>` tag to prevent redundant screen reader noise.

## 2025-05-18 - [Input Group Add-on Misalignment]
**Learning:** Applying fixed column width classes (e.g., col-9) directly to an <input> element inside a Bootstrap .input-group container disrupts flexbox layout and causes add-ons (like password visibility toggles) to misalign or overflow.
**Action:** When adding Bootstrap input-group-text add-ons to inputs, avoid applying fixed column width classes directly to the <input> element. Rely on the container's layout classes instead.
## 2026-04-03 - Hide decorative SVGs inside links with aria-labels
**Learning:** When embedding decorative `<svg>` icons (like Bootstrap icons) inside interactive elements (such as `<a>` tags functioning as buttons) that already have an `aria-label` or descriptive title, screen readers can interpret the SVG itself as readable content, resulting in redundant announcements.
**Action:** Always add `aria-hidden="true"` to the `<svg>` tag within such interactive elements to prevent redundant screen reader noise and provide a cleaner accessibility experience.

## 2026-04-04 - [Contextual Information in Javascript Confirm Dialogs]
**Learning:** Found data tables (like the portmap configuration) using a generic Javascript `confirm()` dialog (e.g., "Are you sure you want to remove this portmap entry?") when deleting rows. This lacks context and can lead to accidental deletions of the wrong entry, especially if a user misclicks.
**Action:** When creating Javascript `confirm()` dialogs for destructive actions within repeating data table rows, always include contextual data (like the row's primary identifier, e.g., the external port) in the message string to ensure users know exactly which item they are acting upon.
## 2026-04-04 - [Screen Reader Noise from Informational SVGs]
**Learning:** Found decorative SVGs (`bi-wifi`, `bi-wifi-off`) that function purely as informational/status icons next to explicit textual status descriptions (like "signal strength: X db") lacking the `aria-hidden="true"` attribute. Even though they weren't inside links or buttons, their lack of `aria-hidden` could still cause screen readers to announce them unnecessarily, confusing the user who already receives the same information through text.
**Action:** Always add `aria-hidden="true"` to informational SVGs used in UI designs alongside their textual counterparts, not just to SVGs inside interactive components like buttons.

## 2024-04-14 - HTML5 Pattern Validation Requires Required Attribute
**Learning:** HTML5 `pattern` attributes for regex validation do not trigger on empty inputs. When applying `pattern` validation to conditionally enabled inputs, they must be paired with the `required` attribute to ensure the browser enforces the validation and prevents empty submissions.
**Action:** Always add the `required` attribute when using `pattern` for form validation on conditionally enabled inputs.
