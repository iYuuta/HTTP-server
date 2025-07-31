#!/usr/bin/env python3

import sys
import os
import urllib.parse

def parse_post_data():
    try:
        content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    except (TypeError, ValueError):
        content_length = 0
    raw_data = sys.stdin.read(content_length)
    return urllib.parse.parse_qs(raw_data)

def html_escape(text):
    return (text.replace("&", "&amp;")
                .replace("<", "&lt;")
                .replace(">", "&gt;")
                .replace('"', "&quot;")
                .replace("'", "&#x27;"))

def main():
    post_data = parse_post_data()
    name = html_escape(post_data.get("name", [""])[0])
    message = html_escape(post_data.get("message", [""])[0])

    print("Content-Type: text/html")
    print()
    print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Form Submitted</title>
</head>
<body>
    <h1>Thank You!</h1>
    <p><strong>Name:</strong> {name}</p>
    <p><strong>Message:</strong> {message}</p>
    <a href="/index.html">Go back</a>
</body>
</html>""")

if __name__ == "__main__":
    main()
