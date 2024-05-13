#pragma once

#include "EventHandler.hpp"
#include <unistd.h>
#include <iostream>
#include <fstream>

class HttpHandler : public EventHandler
{
	private:
		char buffer[1024];
	public:
		HttpHandler();
		HttpHandler(int client_socket);
		HttpHandler(const HttpHandler& other);
		HttpHandler& operator=(const HttpHandler& other);
		int Read();
		int Write();
		EventHandler* Accept();
		~HttpHandler();
};
