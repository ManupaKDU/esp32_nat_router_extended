import re

with open("src/pages/config.html", "r") as f:
    content = f.read()

print(re.search(r'id="ap_ssid"[^>]*>', content).group(0))
print(re.search(r'id="ssid"[^>]*>', content).group(0))
