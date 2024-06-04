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
		Response *Parse(std::vector<ServerConf> confs, ServerConf& handler);
		Method GetRequestType() const;
		int CheckRequestFormed();
		size_t getContentLength();
		ServerConf getServerHandler(std::vector<ServerConf>& confs);
		~RequestParser();
};