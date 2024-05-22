#pragma once

#include "main.hpp"

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
