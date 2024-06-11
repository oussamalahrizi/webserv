#pragma once

#include "common.hpp"
#include "EventHandler.hpp"
#include "ServerConf.hpp"
#include "HttpExceptions.hpp"

enum state
{
	READING,
	BODY,
	DONE
};

typedef struct s_data
{
	ServerConf handler;
	std::map<std::string, std::string> headers;
	std::string body_file;
	std::string uri;
	Method type;
} data;

class HttpHandler : public EventHandler
{
	public:
		int read_state;
	private:
		std::string request;
		size_t content_length;
		int body_fd;
		std::string body;
		clock_t start;
		int status_code;
		std::string status_message;
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
		void openBodyFile();
		int checkTimeout();
		void BuildResponse();
		int buildTimeout();
		void parseHeaders();
		~HttpHandler();
};

data Parse(std::string request, std::vector<ServerConf> &servers, int socket_fd);