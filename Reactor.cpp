#include "Reactor.hpp"

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
	struct epoll_event ep_ev;
	std::string type;

	type = getEventHandlerType(event);
	if (type == "server")
		ep_ev.events = EPOLLIN;
	else
		ep_ev.events = EPOLLIN | EPOLLOUT;
	ep_ev.data.fd = event->getSocketFd();
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, event->getSocketFd(), &ep_ev) < 0)
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
		event_count = epoll_wait(this->epoll_fd, this->ep_events, 100, -1);
		Manage(event_count);
	}
}

void Reactor::RemoveSocket(int socket_fd)
{
	std::cout << "Removing a " << getEventHandlerType(this->map[socket_fd]) << " socket" << std::endl;
	epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, socket_fd, this->ep_events);
	std::string type = getEventHandlerType(this->map[socket_fd]);
	delete this->map[socket_fd];
	this->map.erase(socket_fd);
	close(socket_fd);
}

Reactor::~Reactor()
{
	delete[] this->ep_events;
	close(this->epoll_fd);
}

int f = 0;
void Reactor::Manage(int event_count)
{
	int fd;
	AcceptHandler *server;
	HttpHandler *client;

	for (int i = 0; i < event_count; i++)
	{
		fd = this->ep_events[i].data.fd;
		// fd is ready to read
		if (this->ep_events[i].events & EPOLLIN)
		{
			if ((server = dynamic_cast<AcceptHandler *>(this->map[fd])) != NULL)
			{
				client = dynamic_cast<HttpHandler *>(server->Accept());
				if (client)
				{
					return this->AddSocket(client->getSocketFd(), client);
				}
			}
			else if ((client = dynamic_cast<HttpHandler *>(this->map[fd])) != NULL)
			{
				std::cout << "READING " << std::endl;
				if (client)
				{
					if (client->Read() == 0)
						return this->RemoveSocket(client->getSocketFd());
				}
			}
		}
		// fd is ready to write
		else if (this->ep_events->events & EPOLLOUT)
		{
			std::cout << "WRITING" << std::endl;
			client = dynamic_cast<HttpHandler *>(this->map[fd]);
			if (client)
			{
				// checking timeout
				if ((clock() - client->getStart()) > 5 * CLOCKS_PER_SEC)
					return this->RemoveSocket(client->getSocketFd());
				// handle cgi before writing back the response
				if (client->Write() == 0)
						return this->RemoveSocket(client->getSocketFd());
			}
		}
	}
}