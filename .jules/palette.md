## 2024-05-14 - [Autofocus on Single-Action Forms]
**Learning:** For simple, single-action forms like login or unlock screens, placing the user's cursor immediately inside the primary input field using the HTML5 `autofocus` attribute saves a click and reduces interaction friction.
**Action:** Always add the `autofocus` attribute to the primary input on single-action form pages (e.g., login, unlock).

## 2024-06-12 - [Textual Loading Symbol Accessibility]
**Learning:** When using decorative unicode characters (like `↻`) to simulate loading states where functional CSS spinners are unavailable, wrapping them in `<span aria-hidden="true">` prevents screen readers from awkwardly vocalizing the character. This ensures a cleaner auditory experience while providing visual feedback for sighted users.
**Action:** Always wrap decorative unicode text symbols with `<span aria-hidden="true">` when adding them to the DOM.
## 2024-06-13 - Semantic HTML Tables
**Learning:** Using `<th>` for data cells in tables (like `ota.html` and `otalog.html`) severely degrades accessibility because screen readers announce those cells as row headers, causing confusion for blind users.
**Action:** Always ensure that data cells use `<td>`, and use `<th scope="row">` specifically and only for the leading headers of a row to ensure accurate table navigation and comprehension.
## 2024-06-15 - Dynamic Form Section Accessibility
**Learning:** When dynamically hiding/showing form sections (e.g., `#wpa2-container`) via checkboxes, using `aria-controls` on the toggle and explicitly synchronizing its `aria-expanded` state is crucial for screen reader users. Also, elements within the hidden container (including `input`, `textarea`, and `select`) must be explicitly disabled to prevent accidental submission of hidden data, and this state must be initialized on `document.ready` to reflect any backend-injected initial state.
**Action:** Always add `aria-controls`, bind `aria-expanded` to the toggle's boolean state, explicitly disable *all* child input types when hidden, and trigger the logic on load to sync with the backend.
