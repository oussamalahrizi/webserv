#pragma once

#include "common.hpp"
#include "EventHandler.hpp"
#include "HttpHandler.hpp"

class AcceptHandler : public EventHandler
{

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
