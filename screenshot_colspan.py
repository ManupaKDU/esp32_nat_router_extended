from playwright.sync_api import sync_playwright
import os

def check():
    with sync_playwright() as p:
        browser = p.chromium.launch()
        page = browser.new_page()

        # Build about page with empty states
        about_html = """
        <!DOCTYPE html>
        <html>
        <head>
            <link rel="stylesheet" href="file://{0}/src/pages/styles-67aa3b0203355627b525be2ea57be7bf.css">
        </head>
        <body class="container">
            <h1 class="text-center">About</h1>
            <table class="table table-striped">
                <tbody>
                    <tr>
                        <td colspan="2" class="text-center">ESP32 NAT Router is a simple micro controller based range extender. This
                            can also be used to open a second (guest) Wifi. <p>This is a spare time project. If
                                you have any questions or issues, feel free to ask at the Github project page.
                            </p>
                        </td>
                    </tr>
                </tbody>
            </table>
        </body>
        </html>
        """.format(os.getcwd())

        page.goto(f"file://{os.getcwd()}/src/pages/about.html")
        page.set_content(about_html)
        page.screenshot(path="screenshot_about.png")

        browser.close()

check()
