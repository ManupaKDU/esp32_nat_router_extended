## 2026-06-02 - Textual Loading Indicators for Purged CSS
**Learning:** Standard Bootstrap CSS spinners (`spinner-border`) are purged from the minified CSS and their keyframe animations don't work. Using them results in broken, non-spinning UI elements on long-running waiting pages like `apply.html` and `scan.html`.
**Action:** Enhance waiting states using textual visual loading indicators (e.g., `&#8635; Restarting...`) wrapped in `<span aria-hidden="true">` to prevent screen reader awkwardness, rather than trying to restore purged CSS animations.
