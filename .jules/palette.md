## 2024-05-14 - [Autofocus on Single-Action Forms]
**Learning:** For simple, single-action forms like login or unlock screens, placing the user's cursor immediately inside the primary input field using the HTML5 `autofocus` attribute saves a click and reduces interaction friction.
**Action:** Always add the `autofocus` attribute to the primary input on single-action form pages (e.g., login, unlock).

## 2024-06-12 - [Textual Loading Symbol Accessibility]
**Learning:** When using decorative unicode characters (like `↻`) to simulate loading states where functional CSS spinners are unavailable, wrapping them in `<span aria-hidden="true">` prevents screen readers from awkwardly vocalizing the character. This ensures a cleaner auditory experience while providing visual feedback for sighted users.
**Action:** Always wrap decorative unicode text symbols with `<span aria-hidden="true">` when adding them to the DOM.
## 2024-06-13 - Semantic HTML Tables
**Learning:** Using `<th>` for data cells in tables (like `ota.html` and `otalog.html`) severely degrades accessibility because screen readers announce those cells as row headers, causing confusion for blind users.
**Action:** Always ensure that data cells use `<td>`, and use `<th scope="row">` specifically and only for the leading headers of a row to ensure accurate table navigation and comprehension.

## 2026-06-09 - Add autofocus to unlock password field
**Learning:** For simple, single-action pages like the unlock screen, users have to manually click the password input field before they can start typing, which adds unnecessary friction.
**Action:** Use the HTML5 `autofocus` attribute on the primary input field for such simple forms to immediately place the user's cursor in the right place, saving a click and improving the overall usability flow.

