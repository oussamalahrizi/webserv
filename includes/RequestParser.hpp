#pragma once

#include "main.hpp"



# define DCRLF "\r\n\r\n"
# define CRLF "\r\n"

class Response;

class RequestParser
{
	private:
		std::string request;
		std::map<std::string, std::string> headers;
		std::string uri;
		std::string host;
		Response *response;
		Method type;
	public:
		RequestParser();
		RequestParser(const std::string& input);
		int checkProtocol(std::string &protocol);
		std::vector<std::string> extractLines();
		Response *Parse();
		Method GetRequestType() const;
		int CheckRequestFormed();
		~RequestParser();
};