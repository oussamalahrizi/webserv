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
	public:
		int read_state;
	private:
		std::string request;
		size_t content_length;
		
		std::string body;
		Response *httpResponse;
		RequestParser parser;
		clock_t start;
		int status_code;
		std::string message;
		std::ofstream file;
	public:
		HttpHandler();
		HttpHandler(int client_socket, const std::vector<ServerConf> &ServerConfs);
		HttpHandler(const HttpHandler &other);
		HttpHandler &operator=(const HttpHandler &other);
		int Read();
		int Write();
		int readHeaders();
		int readBody(int flag);
		EventHandler *Accept();
		std::string getFullRequest() const;
		clock_t getStart() const;
		int checkTimeout();
		void BuildResponse();
		int buildTimeout();
		void parseHeaders();
		~HttpHandler();
};
