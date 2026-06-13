## 2024-05-22 - aria-valuenow on progress bars
**Learning:** Found an instance in `otalog.html` where `aria-valuenow` was left empty (`aria-valuenow=""`) while the visual width was being dynamically updated via a format string. This pattern breaks accessibility as screen readers cannot announce the progress value.
**Action:** When updating HTML templates with progress bars, always ensure both the visual `style="width: %d%%"` and the accessible `aria-valuenow="%d"` are populated with the same value using the backend formatting string to keep visual and accessible states synchronized.
