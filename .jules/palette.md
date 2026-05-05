## 2024-05-05 - WPA2 Enterprise Toggle Accessibility & Safety

**Learning:** The WPA2 Enterprise settings container (`#wpa2-container`) in `config.html` contains a `<textarea>` for the certificate. However, the existing JavaScript toggle logic only disabled `input` elements when the container was hidden. This means the hidden certificate `textarea` could still submit its default value, and the toggle switch itself lacked `aria-controls` and `aria-expanded` attributes for screen readers.

**Action:** When dynamically hiding form sections in UI pages, always disable all child form elements (including `<textarea>`) to prevent accidental submission of hidden fields. Additionally, maintain `aria-controls` and `aria-expanded` attributes on the toggle switch for screen reader context. I will update `config.html` to implement this pattern.
