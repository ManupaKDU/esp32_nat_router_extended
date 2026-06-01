## 2024-06-01 - Purged CSS Constraints for Loaders
**Learning:** Standard Bootstrap spinners (`spinner-border`) are non-functional on pages like `apply.html` and `scan.html` because keyframe animations are purged from the minified CSS.
**Action:** When implementing loading states on long-running pages (using `<meta http-equiv="refresh">`), use textual visual loading indicators like `<span aria-hidden="true">↻</span> Restarting...` inside inline elements to maintain valid semantics and prevent screen readers from vocalizing the decorative symbol.
