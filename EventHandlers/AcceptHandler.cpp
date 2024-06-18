#include "../includes/AcceptHandler.hpp"

AcceptHandler::AcceptHandler() : EventHandler(-1) {}

AcceptHandler::AcceptHandler(int socket_fd, const std::vector<ServerConf> &ServerConfs) : EventHandler(socket_fd, ServerConfs) {}

AcceptHandler::AcceptHandler(const AcceptHandler &other) : EventHandler(other)
{
	*this = other;
}

AcceptHandler &AcceptHandler::operator=(const AcceptHandler &other)
{
	if (this != &other)
		EventHandler::operator=(other);
	return *this;
}

AcceptHandler::~AcceptHandler() {}

EventHandler *AcceptHandler::Accept()
{
	struct sockaddr_storage client_address;
	socklen_t addr_len;
	int client_socket;

	addr_len = sizeof(client_address);
	// TODO : find a use of this client address info if needed
	// else make them NULL and look how it may impact the webServerConf
	// for now keeping them for the sake of the function params
	client_socket = accept(this->socket_fd, (struct sockaddr *)&client_address, &addr_len);
	if (client_socket < 0)
	{
		std::cerr << "accept failed" << std::endl;
		return (NULL);
	}
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	return (new HttpHandler(client_socket, this->ServerConfs));
}

void AcceptHandler::Read()
{
}

void AcceptHandler::Write()
{
}