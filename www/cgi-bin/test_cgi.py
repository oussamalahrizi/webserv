#!/usr/bin/env python3

import os
import cgi
import cgitb

# Enable CGI error reporting
cgitb.enable()

# Output headers
print("Content-Type: text/html\r")
print("\r")  # End of headers

# Display the upload form
print('''
<html>
<head><title>File Upload</title></head>
<body>
<h2>Upload a File</h2>
<form enctype="multipart/form-data" method="POST" action="/py/test_cgi.py">
    <input type="file" name="file" required>
    <input type="submit" value="Upload">
</form>
''')

# Process the uploaded file
form = cgi.FieldStorage()
if "file" in form:
    file_item = form["file"]
    if file_item.filename:
        # Save the uploaded file in the upload directory
        upload_path = os.path.join("/cgi-bin", os.path.basename(file_item.filename))
        
        # Open the file in binary write mode
        with open(upload_path, "wb") as output_file:
            output_file.write(file_item.file.read())  # Write the binary data

        print(f"<h3>File '{file_item.filename}' uploaded successfully!</h3>")
        print(f"<p>Saved to: {upload_path}</p>")
    else:
        print("<h3>No file was uploaded.</h3>")
print('</body></html>')
