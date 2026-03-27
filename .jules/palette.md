## 2023-10-25 - [Empty States and Inline Validation]
**Learning:** Found empty state messages in tables styled with `text-danger` and `text-warning`, making normal zero-state conditions appear as system errors. Missing `required` attributes on add-item forms allowed invalid partial submissions.
**Action:** When creating empty states, use neutral styling (like `text-muted`) to avoid causing user alarm. Always leverage HTML5 `required` attributes for simple inline validation before resorting to backend handling.

## $(date +%Y-%m-%d) - Add ARIA-labeled password toggle
**Learning:** Adding Bootstrap `input-group-text` add-ons (such as password visibility toggle icons) to inputs can break if fixed column width classes (e.g., `col-9`) are applied directly to the `<input>` element. This disrupts flexbox layout within the `.input-group` container and causes add-ons to misalign or overflow.
**Action:** When adding input-groups, always apply sizing classes to a wrapper `<div>` rather than directly to the `<input>` element to maintain responsive flex layouts.
