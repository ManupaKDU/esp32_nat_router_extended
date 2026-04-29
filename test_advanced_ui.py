import sys
import re
from playwright.sync_api import sync_playwright

def main():
    with open('src/pages/advanced.html', 'r') as f:
        html_template = f.read()

    # Fill in the string placeholders %s and %d with dummy data
    # The advanced.html uses sprintf with these arguments:
    # advanced_start, hostName, octet, lowSelected, mediumSelected, highSelected, bwHigh, bwLow, ledCB, aliveCB, natCB, currentDNS, defCB, cloudCB, adguardCB, customCB, customDNSIP, currentMAC, defMacCB, defaultMAC, rndMacCB, subMac, customMacCB, customMac, netmask, classCCB, octet, classBCB, octet, classACB, octet, customMaskCB, customMask

    # Simple substitution via string format matching the exact sequence of %s and %d from advancedhandler.c:193
    # Just replacing all %s with '' and %d with 1 is a bit error prone if we miss something.
    # Let's substitute all %s with empty string and %d with 1.

    html_content = re.sub(r'%s', '', html_template)
    html_content = re.sub(r'%d', '1', html_content)

    # We want to enable the custom radio buttons to test the 'required' field behavior.
    # We will simulate this by checking the radio boxes through playwright.

    html_file = 'test_advanced_ui.html'
    with open(html_file, 'w') as f:
        f.write(html_content)

    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        context = browser.new_context(record_video_dir="videos/")
        page = context.new_page()

        page.goto(f'file://{sys.path[0]}/{html_file}')

        # Test Custom DNS IP requirement
        page.check('#custom')
        page.click('input[value="Apply"]')

        # We can't easily catch the browser validation popup in headless without some work, but we can take a screenshot
        # of the form with the radio buttons selected.

        page.screenshot(path="advanced_ui_verification.png")

        context.close()
        browser.close()

if __name__ == '__main__':
    main()
