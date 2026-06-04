## 2024-06-25 - Accessible Visual Indicators for Purged CSS
**Learning:** Standard Bootstrap `spinner-border` keyframe animations are purged from the minified CSS in this project.
**Action:** When implementing loading indicators, use textual visual indicators (e.g., `↻ Restarting...`) wrapped in `<span aria-hidden="true">` to maintain visual cues while preventing awkward vocalizations by screen readers.
