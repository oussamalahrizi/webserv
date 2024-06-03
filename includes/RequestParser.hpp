#pragma once

#include "main.hpp"



# define DCRLF "\r\n\r\n"
# define CRLF "\r\n"


class RequestParser
{
	public:
		RequestParser();
		static std::map<std::string, std::string> Parse(std::string& request);
		static Method GetRequestType(std::string& request);
		~RequestParser();
};