#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include "Reactor.hpp"
#include "configParser/includes/ConfigParser.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include <algorithm>