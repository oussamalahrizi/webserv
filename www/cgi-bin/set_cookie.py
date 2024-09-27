#!/usr/bin/env python3

from http import cookies
import os
import sys
import urllib.parse

# Get the content length to read POST data
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
post_data = sys.stdin.read(content_length)

# Parse the POST data
form_data = urllib.parse.parse_qs(post_data)
cookie_value = form_data.get('cookie_value', [''])[0]  # Get the cookie_value from the form
cookie_key = form_data.get('cookie_key', [''])[0]  # Get the cookie_value from the form

# Set the content type for the response
print("Content-Type: text/html\r")
print(f"Set-Cookie: {cookie_key}={cookie_value}\r")
print("\r")  # Blank line indicating end of headers

# HTML response
print("<html>")
print("<head><title>Cookie Received</title></head>")
print("<body>")
print(f"<p>Cookie Key Posted: {cookie_key}</p>")
print(f"<p>Cookie Value Posted: {cookie_value}</p>")
print("<a href='/py'>Back to Cookie Test</a>")
print("</body>")
print("</html>")
