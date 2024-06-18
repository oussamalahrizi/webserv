#pragma once

#include "common.hpp"
#include "EventHandler.hpp"
#include "ServerConf.hpp"
#include "HttpExceptions.hpp"

enum state
{
	READ,
	WRITE,
	CLOSE
};

enum
{
	CHUNKED,
	LENGTH
};

# define READ_SIZE 65536

class ChunkedBody;

typedef struct s_data
{
	ServerConf handler;
	Location loc;
	std::map<std::string, std::string> headers;
	std::string uri;
	std::string ressource;
	Method type;
	int trans;
	std::string request;
	std::string tempfile_name;
	int temp_fd;
} data;

class HttpHandler : public EventHandler
{
	private:
		clock_t start;
		int read_state;
		int headers_done;
		data m_data;
		std::string rest;
		int status_code;
		int throwing;
	public:
		HttpHandler();
		HttpHandler(int client_socket, const std::vector<ServerConf> &ServerConfs);
		HttpHandler(const HttpHandler &other);
		HttpHandler &operator=(const HttpHandler &other);
		void Read();
		void Write();
		void throwBody();
		int handleEvent(uint32_t event);
		void readHeaders();
		EventHandler* Accept();
		void BuildResponse();
		~HttpHandler();
};

void Parse(std::string request, std::vector<ServerConf> &servers, int socket_fd, data& result);

#include "ChunkedBody.hpp"