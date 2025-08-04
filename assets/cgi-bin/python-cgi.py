#!/usr/bin/env python3

import os
import sys
import urllib.parse

def html_escape(s):
    return (s.replace("&", "&amp;")
             .replace("<", "&lt;")
             .replace(">", "&gt;")
             .replace('"', "&quot;")
             .replace("'", "&#x27;"))

def get_post_data():
    try:
        content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    except (ValueError, TypeError):
        content_length = 0
    return sys.stdin.read(content_length)

def parse_form_data(data):
    return urllib.parse.parse_qs(data)

print("Content-Type: text/html\r\n")

print("<!DOCTYPE html>")
print("<html><head><title>CGI Response</title></head><body>")
print("<h1>Hello from CGI!</h1>")

method = os.environ.get("REQUEST_METHOD", "GET")
print(f"<p>Request Method: <strong>{method}</strong></p>")

if method == "GET":
    query = os.environ.get("QUERY_STRING", "")
    print(f"<p>Query String: {html_escape(query)}</p>")
    parsed = urllib.parse.parse_qs(query)
elif method == "POST":
    raw_data = get_post_data()
    print(f"<p>Raw POST Data: {html_escape(raw_data)}</p>")
    parsed = parse_form_data(raw_data)
else:
    parsed = {}

if parsed:
    print("<h2>Parsed Form Data:</h2><ul>")
    for key, values in parsed.items():
        for v in values:
            print(f"<li>{html_escape(key)} = {html_escape(v)}</li>")
    print("</ul>")

print("</body></html>")
