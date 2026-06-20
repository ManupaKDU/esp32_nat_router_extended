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


## 2024-05-22 - aria-valuenow on progress bars
**Learning:** Found an instance in `otalog.html` where `aria-valuenow` was left empty (`aria-valuenow=""`) while the visual width was being dynamically updated via a format string. This pattern breaks accessibility as screen readers cannot announce the progress value.
**Action:** When updating HTML templates with progress bars, always ensure both the visual `style="width: %d%%"` and the accessible `aria-valuenow="%d"` are populated with the same value using the backend formatting string to keep visual and accessible states synchronized.


## 2026-06-06 - Password Visibility Toggles
**Learning:** Adding password visibility toggles should be implemented using vanilla JavaScript rather than relying on external dependencies like jQuery, to keep the UX improvements self-contained and avoid adding unnecessary libraries to simple pages. The toggle UI component consists of an `input-group` wrapper and a clickable `span` with an eye SVG icon.
**Action:** When adding new password fields or modifying existing ones, ensure the input is wrapped in `.input-group`, the eye icon `span` is present, and a vanilla JS event listener is included to toggle the `type` attribute between `text` and `password`.

