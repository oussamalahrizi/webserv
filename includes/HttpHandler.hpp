#pragma once

#include "common.hpp"
#include "EventHandler.hpp"
#include "ServerConf.hpp"
#include "HttpExceptions.hpp"
#include "UUID.hpp"

enum state
{
	READ,
	BODY,
	WRITE,
	CLOSE
};

enum
{
	CHUNKED,
	LENGTH,
	NONE
};

# define READ_SIZE 65536

class ChunkedBody;
class LengthBody;


typedef struct s_data
{
	ServerConf handler;
	Location loc;
	int serv_root;
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
		ChunkedBody *chunked;
		LengthBody *cl;
	public:
		HttpHandler();
		HttpHandler(int client_socket, const std::vector<ServerConf> &ServerConfs);
		HttpHandler(const HttpHandler &other);
		HttpHandler &operator=(const HttpHandler &other);
		void Read();
		void Write();
		int handleEvent(uint32_t event);
		void readHeaders();
		EventHandler* Accept();
		void openTempFile();
		void handleBody();
		const std::string& getRequest() { return this->m_data.request;}
		const clock_t& getStart() { return this->start;}
		int getState() { return this->read_state;}
		~HttpHandler();
};

void Parse(std::string request, std::vector<ServerConf> &servers, int socket_fd, data& result);

#include "ChunkedBody.hpp"
#include "LengthBody.hpp"