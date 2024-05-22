#pragma once

#include "main.hpp"

enum Method
{
	GET,
	POST,
	DELETE
};

# define DCRLF "\r\n\r\n"
# define CRLF "\r\n"


class RequestParser
{
	private:
		std::map<std::string, std::string> headers;
	public:
		RequestParser();
		int Parse(std::string& request);
		std::map<std::string, std::string> GetHeaders() const;
		static Method GetRequestType(std::string& request);
		~RequestParser();
};