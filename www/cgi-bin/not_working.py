
# 502
def wrong_status_code():
	print("HTTP/1.1 404 mamak\r")
	print("\r")

# 200 OK
def nostatus_noresline():
	print("Test-Header: alo\r")
	print("\r")
# 502
def missing_httpline():
	print("HTTP/1.0 200 OK\r")
	print("\r")

import json

def HelloJson():
	msg = {
		"response":"Hello There!",
		"int_test" : 24
	}
	msg = json.dumps(msg)
	print("HTTP/1.1 200 OK\r")
	print("Content-Type: application/json\r")
	print(f"Content-Length: {len(msg)}\r")
	print("\r")
	print(msg)

# missing_httpline()
# HelloJson()
# nostatus_noresline()
# wrong_status_code()