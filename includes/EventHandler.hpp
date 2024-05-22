#pragma once

#include "main.hpp"

class EventHandler
{
protected:
	int socket_fd;

public:
	EventHandler();
	EventHandler(const EventHandler &other);
	EventHandler &operator=(const EventHandler &other);
	EventHandler(int socket_fd);
	int getSocketFd() const;
	virtual int Read() = 0;
	virtual int Write() = 0;
	virtual EventHandler *Accept() = 0;
	virtual ~EventHandler();
};
