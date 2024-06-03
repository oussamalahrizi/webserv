#pragma once

#include "main.hpp"

enum state
{
	HEADERS,
	BODY,
	DONE
};

class HttpHandler : public EventHandler
{
private:
	int read_state;
	std::string request;
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
	int parseHeaders();
	~HttpHandler();
};
