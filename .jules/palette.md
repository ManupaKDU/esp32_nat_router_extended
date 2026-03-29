## 2023-10-25 - [Empty States and Inline Validation]
**Learning:** Found empty state messages in tables styled with `text-danger` and `text-warning`, making normal zero-state conditions appear as system errors. Missing `required` attributes on add-item forms allowed invalid partial submissions.
**Action:** When creating empty states, use neutral styling (like `text-muted`) to avoid causing user alarm. Always leverage HTML5 `required` attributes for simple inline validation before resorting to backend handling.

## 2025-03-27 - [Contextual ARIA Labels for Data Table Buttons]
**Learning:** Found data tables (like Wi-Fi scan results) using identical, generic button labels (e.g., "Use") across rows. This creates accessibility issues for screen reader users who hear repeating generic text without context when tabbing through interactive elements.
**Action:** When creating action buttons within repeating data table rows, always provide an `aria-label` that includes contextual data (like the row's primary identifier, e.g., `aria-label='Use MyWiFi'`) to ensure screen reader clarity.

## 2025-03-31 - [Hiding Decorative SVG Icons from Screen Readers]
**Learning:** Found decorative `<svg>` icons (like the Wi-Fi icon, view password eye, and the add/remove portmap icons) without `aria-hidden="true"`. Because these elements were frequently used within components that already had `aria-label`s or associated text, screen readers were reading the inner SVG elements aloud, creating noise.
**Action:** When creating components with SVG icons, always add `aria-hidden="true"` to the decorative `<svg>` tag if the icon does not convey any unique meaning not already covered by surrounding text or its container's `aria-label`.
