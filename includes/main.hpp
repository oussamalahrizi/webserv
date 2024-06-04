#pragma once

enum Method
{
	GET,
	POST,
	DELETE,
	OTHER
};

# define TIMEOUT_HEADERS 20

#include <vector>
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
#include "ServerConf.hpp"
#include "Location.hpp"
#include "RequestParser.hpp"
#include "EventHandler.hpp"
#include "Response.hpp"
#include "AcceptHandler.hpp"
#include "HttpHandler.hpp"
#include "Reactor.hpp"
#include "Utils.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"
