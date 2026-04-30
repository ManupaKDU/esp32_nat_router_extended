import asyncio
from playwright.async_api import async_playwright
import os

async def main():
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=True)
        context = await browser.new_context(record_video_dir="videos/")
        page = await context.new_page()

        # Load the reset.html file content
        with open("src/pages/reset.html", "r") as f:
            html_content = f.read()

        # Use absolute path to route the file locally
        file_url = f"file://{os.path.abspath('src/pages/reset.html')}"

        await page.goto(file_url)

        # Take a screenshot before clicking
        await page.screenshot(path="screenshot_before.png", full_page=True)

        # Set up a dialog handler to dismiss it
        page.once("dialog", lambda dialog: dialog.dismiss())

        # Click the Erase flash button
        await page.click("input[value='Erase flash']")

        # Take another screenshot
        await page.screenshot(path="screenshot_after.png", full_page=True)

        await context.close()
        await browser.close()

if __name__ == "__main__":
    asyncio.run(main())
