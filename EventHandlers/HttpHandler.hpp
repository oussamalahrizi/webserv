#pragma once

#include "EventHandler.hpp"
#include <unistd.h>
#include <iostream>
#include "../Response.hpp"
#include <fstream>

#include "../Requests/includes/RequestParser.hpp"

class HttpHandler : public EventHandler
{
	private:
		std::string request;
		Response *httpResponse;
		RequestParser parser;
		clock_t start;
	public:
		HttpHandler();
		HttpHandler(int client_socket);
		HttpHandler(const HttpHandler& other);
		HttpHandler& operator=(const HttpHandler& other);
		int Read();
		int Write();
		EventHandler* Accept();
		std::string getFullRequest() const;
		clock_t getStart() const;
		~HttpHandler();
};
