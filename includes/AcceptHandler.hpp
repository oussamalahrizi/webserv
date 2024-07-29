#pragma once

#include "common.hpp"
#include "EventHandler.hpp"

typedef struct
{
	std::string client_ip;
	int client_port;
} client_info;

class HttpHandler; 

class AcceptHandler : public EventHandler
{
	private:
		client_info info;
	private:
		void getClientInfo(struct sockaddr_storage &client_address);
	public:
		AcceptHandler();
		AcceptHandler(const AcceptHandler &other);
		AcceptHandler &operator=(const AcceptHandler &other);
		AcceptHandler(int socket_fd, const std::vector<ServerConf> &ServerConfs);
		void Read();
		void Write();
		EventHandler *Accept();
		~AcceptHandler();
};

#include "HttpHandler.hpp"

