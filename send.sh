#curl -X POST --data-binary @/home/exiled/video.mp4 -H "Host: localhost:3000" http://localhost:3000/txt

#!/bin/bash

# Connect to the server
echo "GET / HTTP/1.1" | nc 127.0.0.1 8080

# Close the connection abruptly
sleep 1
kill -9 $(pgrep nc)
