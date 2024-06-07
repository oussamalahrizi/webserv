#pragma once

#include "main.hpp"

enum state
{
	READING,
	BODY,
	DONE
};

class HttpHandler : public EventHandler
{
private:
	int read_state;
	std::string request;
	size_t content_length;
	
	std::string body;
	Response *httpResponse;
	RequestParser parser;
	clock_t start;
	int status_code;
	std::string message;
public:
	HttpHandler();
	HttpHandler(int client_socket, const std::vector<ServerConf> &ServerConfs);
	HttpHandler(const HttpHandler &other);
	HttpHandler &operator=(const HttpHandler &other);
	int Read();
	int Write();
	EventHandler *Accept();
	std::string getFullRequest() const;
	clock_t getStart() const;
	int checkTimeout();
	void BuildResponse();
	int buildTimeout();
	void parseHeaders();
	~HttpHandler();
};
