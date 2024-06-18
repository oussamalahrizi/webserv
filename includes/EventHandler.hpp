#pragma once

#include "common.hpp"
#include "ServerConf.hpp"

class EventHandler
{
	protected:
		int socket_fd;
		std::vector<ServerConf> ServerConfs;

	public:
		EventHandler();
		EventHandler(const EventHandler &other);
		EventHandler &operator=(const EventHandler &other);
		EventHandler(int socket_fd, const std::vector<ServerConf> &ServerConfs);
		EventHandler(int socket_fd);
		int getSocketFd() const;
		virtual void Read() = 0;
		virtual void Write() = 0;
		virtual EventHandler *Accept() = 0;
		virtual ~EventHandler();
};
