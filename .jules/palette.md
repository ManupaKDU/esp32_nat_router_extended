## 2024-05-18 - Accessible Loading Indicators
**Learning:** When standard CSS spinners (like Bootstrap's `spinner-border`) are unavailable or purged, using standard textual representations like "↻ Restarting..." is a good alternative. However, standard unicode symbols are vocalized awkwardly by screen readers.
**Action:** When implementing textual loading symbols to replace non-functional CSS spinners, wrap the unicode symbol in `<span aria-hidden="true">` to prevent screen readers from awkwardly vocalizing the character, ensuring a cleaner auditory experience.
