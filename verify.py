import os
import time
from playwright.sync_api import sync_playwright

def verify():
    # Make sure output directory exists
    os.makedirs('verification', exist_ok=True)

    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        context = browser.new_context(record_video_dir="verification/", record_video_size={"width": 1280, "height": 720})
        page = context.new_page()

        print("Testing ota.html...")
        page.goto("http://localhost:8000/ota.html")
        time.sleep(2)
        # Verify the semantic markup is present by checking the th tags
        page.wait_for_selector("th[scope='row']")

        page.screenshot(path="verification/ota.png")

        print("Testing otalog.html...")
        page.goto("http://localhost:8000/otalog.html")
        time.sleep(2)
        # Verify there is no 'th' for OTA update start text by looking at td instead
        page.wait_for_selector("td:has-text('OTA update started with')")

        page.screenshot(path="verification/otalog.png")

        # Close to save video
        context.close()
        browser.close()

        # Rename the video
        video_files = [f for f in os.listdir("verification") if f.endswith(".webm")]
        if video_files:
            os.rename(os.path.join("verification", video_files[0]), "verification/semantic_tables.webm")

        print("Verification completed successfully.")

if __name__ == "__main__":
    verify()
