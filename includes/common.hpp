#pragma once

enum Method
{
	GET,
	POST,
	DELETE,
	OTHER
};


# define TIMEOUT_HEADERS 20
# define DCRLF "\r\n\r\n"
# define CRLF "\r\n"

#include <vector>
#include <climits>
#include <map>
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <algorithm>
#include <exception>
#include <netinet/in.h>
#include <arpa/inet.h>

extern std::map<std::string, std::string> mimetype;

extern std::map<int, std::string> http_codes;