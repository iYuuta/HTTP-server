#!/usr/bin/env python3
import sys

# Body content
body = "<html><body><h1>Hello, CGI!</h1></body></html>"

# Encode body to bytes to get the precise length
body_bytes = body.encode('utf-8')
content_length = len(body_bytes)

# Output headers
print(f"Content-Type: text/html; charset=utf-8\n")
# print(f"Content-Length: {content_length}")
print()  # End of headers

# Output body
sys.stdout.buffer.write(body_bytes)
