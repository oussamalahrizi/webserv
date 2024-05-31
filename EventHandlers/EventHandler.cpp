#include "../includes/main.hpp"

EventHandler::EventHandler() {}

EventHandler::EventHandler(int socket_fd, const std::vector<Server>& servers) : socket_fd(socket_fd), servers(servers) {}

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