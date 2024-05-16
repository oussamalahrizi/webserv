#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include "Reactor.hpp"
#include "configParser/includes/ConfigParser.hpp"

int main(int ac, char **av)
{
	ConfigParser conf;
	if (ac == 2)
	{
		try
		{
			conf = ConfigParser(av[1]);
			return (0);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
	struct addrinfo hints;
	struct addrinfo *bind_address;
	int socket_fd;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	const char * host = conf.conf.host.c_str();
	const char * port = conf.conf.ports[0].c_str();
	if (getaddrinfo(host, port, &hints , &bind_address))
	{
		std::cout << "getaddinfo failed" << std::endl;
		exit(1);
	}
	
	socket_fd = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (socket_fd < 0)
	{
		std::cout << "socket failed" << std::endl;
		exit(1);
	}
	int resure = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &resure, sizeof(resure)))
	{
		std::cout << "setsocketopt failed" << std::endl;
		exit(1);
	}

	if (bind(socket_fd, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		std::cout << "bind failed" << std::endl;
		exit(1);
	}

	free(bind_address);
	listen(socket_fd, 100);

	Reactor reactor;
	reactor.AddSocket(socket_fd, new AcceptHandler(socket_fd));
	reactor.EventPool();

	return (0);
}
