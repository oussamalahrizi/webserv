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
cookie_value1 = form_data.get('cookie_value1', [''])[0]  # Get the cookie_value from the form
cookie_key1 = form_data.get('cookie_key1', [''])[0]  # Get the cookie_value from the form
cookie_value2 = form_data.get('cookie_value2', [''])[0]  # Get the cookie_value from the form
cookie_key2 = form_data.get('cookie_key2', [''])[0]  # Get the cookie_value from the form

# Set the content type for the response
print("Content-Type: text/html\r")
print(f"Set-Cookie: {cookie_key1}={cookie_value1}\r")
print(f"Set-Cookie: {cookie_key2}={cookie_value2}\r")
print("\r")  # Blank line indicating end of headers

# HTML response
print("<html>")
print("<head><title>Cookie Received</title></head>")
print("<body>")
print(f"<p>Cookie Key Posted: {cookie_key1}</p>")
print(f"<p>Cookie Value Posted: {cookie_value1}</p>")
print(f"<p>Cookie Key Posted: {cookie_key2}</p>")
print(f"<p>Cookie Value Posted: {cookie_value2}</p>")
print("<a href='/py'>Back to Cookie Test</a>")
print("</body>")
print("</html>")
