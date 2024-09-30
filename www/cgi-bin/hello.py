text = "hello world"
print("HTTP/1.1 200 OK\r")
print(f"Content-length: {len(text)}\r")
print("Content-type: text/html\r")
print("\r")
print(text)