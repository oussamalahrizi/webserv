#include "../includes/AcceptHandler.hpp"
#include <cerrno>
#include <ctime>
#include <ostream>

AcceptHandler::AcceptHandler() : EventHandler(-1) {}

AcceptHandler::AcceptHandler(int socket_fd, const std::vector<ServerConf> &ServerConfs) : EventHandler(socket_fd, ServerConfs)
{
	info.client_ip = "";
	info.client_port = -1;
}

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

void AcceptHandler::getClientInfo(struct sockaddr_storage &client_address)
{
	char ip[INET6_ADDRSTRLEN];
	if (client_address.ss_family == AF_INET)
	{
	    struct sockaddr_in *s = (struct sockaddr_in *)&client_address;
	    inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));
	    info.client_port = (int) ntohs(s->sin_port);
		info.client_ip = ip;
	}
	else if (client_address.ss_family == AF_INET6)
	{
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&client_address;
	    inet_ntop(AF_INET6, &s->sin6_addr, ip, sizeof(ip));
	    info.client_port = (int) ntohs(s->sin6_port);
		info.client_ip = ip;
	}
}

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
    getClientInfo(client_address);
	return (new HttpHandler(client_socket, this->ServerConfs, info));
}

void AcceptHandler::Read()
{
}

void AcceptHandler::Write()
{
}