#pragma once

#include "EventHandler.hpp"
#include <unistd.h>
#include <iostream>
#include "../Response.hpp"
#include <fstream>

class HttpHandler : public EventHandler
{
	private:
		std::string request;
		Response *httpResponse;
	public:
		HttpHandler();
		HttpHandler(int client_socket);
		HttpHandler(const HttpHandler& other);
		HttpHandler& operator=(const HttpHandler& other);
		int Read();
		int Write();
		EventHandler* Accept();
		std::string getFullRequest() const;
		~HttpHandler();
};
