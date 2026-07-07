import os
from playwright.sync_api import sync_playwright

def test_autofocus():
    with sync_playwright() as p:
        browser = p.chromium.launch()
        page = browser.new_page()

        # Test unlock.html
        unlock_path = os.path.abspath('src/pages/unlock.html')
        page.goto(f"file://{unlock_path}")

        # Give a small delay for any rendering/focus
        page.wait_for_timeout(500)

        focused_element = page.evaluate("() => document.activeElement.id")
        assert focused_element == "unlock", f"Expected 'unlock' to be focused, but got {focused_element}"
        print("✅ unlock.html autofocus is working!")

        # Test lock.html
        lock_path = os.path.abspath('src/pages/lock.html')
        page.goto(f"file://{lock_path}")

        page.wait_for_timeout(500)

        focused_element = page.evaluate("() => document.activeElement.id")
        assert focused_element == "lockpass", f"Expected 'lockpass' to be focused, but got {focused_element}"
        print("✅ lock.html autofocus is working!")

        browser.close()

if __name__ == "__main__":
    test_autofocus()
