#include "EventHandler.hpp"

EventHandler::EventHandler() {}

EventHandler::EventHandler(int socket_fd) : socket_fd(socket_fd) {}

EventHandler::EventHandler(const EventHandler &other)
{
	*this = other;
}

EventHandler::~EventHandler() {}

EventHandler &EventHandler::operator=(const EventHandler &other)
{
	if (this != &other)
		this->socket_fd = other.socket_fd;
	return (*this);
}

int EventHandler::getSocketFd() const
{
	return this->socket_fd;
}