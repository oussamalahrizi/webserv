#pragma once

#include "EventHandler.hpp"
#include "HttpHandler.hpp"
#include <sys/socket.h>
#include <fcntl.h>

class AcceptHandler : public EventHandler
{
	public:
		AcceptHandler();
		AcceptHandler(const AcceptHandler& other);
		AcceptHandler& operator=(const AcceptHandler& other);
		AcceptHandler(int socket_fd);
		int Read();
		int Write();
		EventHandler* Accept();
		~AcceptHandler();
};
