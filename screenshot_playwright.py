from playwright.sync_api import sync_playwright
import os
import sys

def verify_colspan_tables():
    with sync_playwright() as p:
        browser = p.chromium.launch()
        page = browser.new_page()

        # Let's mock a page that has tables with empty states
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <link rel="stylesheet" href="file://{os.getcwd()}/src/pages/styles-67aa3b0203355627b525be2ea57be7bf.css">
        </head>
        <body class="container">
            <h1 class="text-center">Connected clients</h1>
            <table class="table table-striped" aria-label="Connected clients list">
                <thead class="text-center fw-bold">
                    <tr>
                        <th scope="col" class=fw-bold>#</th>
                        <th scope="col" class=fw-bold>IP address</th>
                        <th scope="col" class=fw-bold>MAC</th>
                    </tr>
                </thead>
                <tbody class="text-center">
                    <tr class='text-muted'><td colspan='3'>No clients connected (NOT CENTERED)</td></tr>
                    <tr class='text-muted text-center'><td colspan='3'>No clients connected (CENTERED via class)</td></tr>
                </tbody>
            </table>
        </body>
        </html>
        """

        page.goto(f"file://{os.getcwd()}/src/pages/clients.html") # navigate to set base url
        page.set_content(html_content)

        page.screenshot(path="screenshot_tables.png")
        browser.close()

verify_colspan_tables()
