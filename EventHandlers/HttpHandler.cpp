#include "HttpHandler.hpp"

HttpHandler::HttpHandler() : EventHandler(-1) {}

HttpHandler::HttpHandler(int client_fd) : EventHandler(client_fd) {}

HttpHandler::HttpHandler(const HttpHandler& other) : EventHandler(other)
{
	*this = other;
}

HttpHandler& HttpHandler::operator=(const HttpHandler& other)
{
	if (this != &other)
		EventHandler::operator=(other);
	return *this;
}

HttpHandler::~HttpHandler() {}

int HttpHandler::Read()
{
	std::string name("test");
	std::ofstream file(name.c_str());

	int readed = read(this->socket_fd, buffer, 1024);
	std::cout << readed << std::endl;
	name = buffer;
	file << name;
	if (readed <= 0)
		return (0);
	return (readed);
}

int HttpHandler::Write()
{
	write(this->socket_fd, "Hello there\n", 13);
	return (0);
}

EventHandler* HttpHandler::Accept()
{
	return (NULL);
}
