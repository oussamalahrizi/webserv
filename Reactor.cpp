#include "includes/common.hpp"
#include "includes/Reactor.hpp"

Reactor::Reactor()
{
	this->epoll_fd = epoll_create1(0);
	if (this->epoll_fd == -1)
		throw std::runtime_error("epoll failed to create instance");
	this->ep_events = new struct epoll_event[100];
}

std::string getEventHandlerType(EventHandler *event)
{
	std::string type;
	type = dynamic_cast<AcceptHandler *>(event) != NULL ? "server" : "client";
	return (type);
}

void Reactor::AddSocket(int socket_fd, EventHandler *event)
{
	std::string type;

	type = getEventHandlerType(event);
	if (type == "server")
		ep_event.events = EPOLLIN;
	else
		ep_event.events = EPOLLIN | EPOLLOUT;
	ep_event.data.fd = socket_fd;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, socket_fd, &ep_event) < 0)
	{
		std::cout << "epoll ctl failed" << std::endl;
		throw std::runtime_error("error");
	}
	this->map[socket_fd] = event;
	std::cout << "Adding a " << type << " socket" << std::endl;
}

void Reactor::EventPool()
{
	int event_count;
	while (true)
	{
		event_count = epoll_wait(this->epoll_fd, this->ep_events, 1024, -1);
		Manage(event_count);
	}
}

void Reactor::RemoveSocket(int socket_fd)
{
	std::cout << "Removing a " << getEventHandlerType(this->map[socket_fd]) << " socket" << std::endl;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL) < 0)
		throw std::runtime_error("delete client error");
	std::string type = getEventHandlerType(this->map[socket_fd]);
	delete this->map[socket_fd];
	this->map.erase(socket_fd);
	close(socket_fd);
}

Reactor::~Reactor()
{
	delete[] this->ep_events;
	close(this->epoll_fd);
	std::map<int, EventHandler*>::iterator it = this->map.begin();
	while (it != this->map.end())
	{
		delete it->second;
		close(it->first);
		it++;
	}
	this->map.clear();
}

int Reactor::isServer(int fd, uint32_t event)
{
	AcceptHandler *server;
	server = dynamic_cast<AcceptHandler *>(this->map[fd]);
	if (server != NULL && (event & EPOLLIN))
		return (1);
	return (0);
}


void Reactor::Manage(int event_count)
{
	int fd;
	AcceptHandler *server;
	HttpHandler *client;

	for (int i = 0; i < event_count; i++)
	{
		fd = this->ep_events[i].data.fd;
		if (this->isServer(fd, this->ep_events[i].events))
		{
			// std::cout << "reading" << std::endl;
			server = dynamic_cast<AcceptHandler *>(this->map[fd]);
			client = dynamic_cast<HttpHandler *>(server->Accept());
			if (client)
				this->AddSocket(client->getSocketFd(), client);
		}
		else
		{
			// std::cout << "writing" << std::endl;
			client = dynamic_cast<HttpHandler *>(this->map[fd]);
			if (client->handleEvent(this->ep_events[i].events) == CLOSE)
				RemoveSocket(client->getSocketFd());
		}
	}
}