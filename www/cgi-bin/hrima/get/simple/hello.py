

msg = "Hello from python CGI"


print("HTTP/1.0 200 OK\r")
print("Content-Type: text/html\r")
print("Content-Length: " + str(len(msg)) + "\r" )
print("\r")
print(msg, end="")