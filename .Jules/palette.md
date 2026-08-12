## 2024-08-12 - Accessibility Alert Anti-Pattern
**Learning:** Using `role="alert"` (or Bootstrap alert classes that include it by default in some frameworks) for static form helper text causes screen readers to spam the user with the description immediately on page load, rather than when the input receives focus.
**Action:** Remove `role="alert"` from static form descriptions and properly associate them with their respective inputs using `aria-describedby="<id>"`.
