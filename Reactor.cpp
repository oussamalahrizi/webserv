#include "Reactor.hpp"


Reactor::Reactor()
{
	this->epoll_fd = epoll_create1(0);
	if (this->epoll_fd == -1)
		throw std::runtime_error("epoll failed to create instance");
	this->ep_events = new struct epoll_event[100];
}

std::string getEventHandlerType(EventHandler* event)
{
	std::string type;
	type = dynamic_cast<AcceptHandler*>(event) != NULL ? "server" : "client";
	return (type);
}

void Reactor::AddSocket(int socket_fd, EventHandler* event)
{
	struct epoll_event ep_ev;

	if (getEventHandlerType(event) == "server")
		ep_ev.events = EPOLLIN;
	else
		ep_ev.events = EPOLLIN | EPOLLOUT;
	ep_ev.data.fd = event->getSocket_fd();
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, event->getSocket_fd(), &ep_ev) < 0)
	{
		std::cout << "epoll ctl failed" << std::endl;
		throw std::runtime_error("error");
	}
	this->map[socket_fd] = event;
	std::cout << "Adding a " << getEventHandlerType(event) << " socket" << std::endl;
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
	delete this->map[socket_fd];
	this->map.erase(socket_fd);
	close(socket_fd);
}

Reactor::~Reactor()
{
	delete [] this->ep_events;
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
		std::cout << this->ep_events[i].events << std::endl;
		// fd is ready to read
		if (this->ep_events[i].events & EPOLLIN)
		{
			server = dynamic_cast<AcceptHandler*>(this->map[fd]);
			if (server)
			{
				client = dynamic_cast<HttpHandler* >(server->Accept());
				if (client)
				{
					return this->AddSocket(client->getSocket_fd(), client);
				}
			}
			else if ((client = dynamic_cast<HttpHandler*>(this->map[fd])) != NULL)
			{
				std::cout << "client ready to read" << std::endl;
				if (client)
				{
					char buffer[1025];
					int readed = read(client->getSocket_fd(), buffer, 1024);
					f= 1;
					buffer[readed] = 0;
					std::cout << buffer << std::endl;
					return this->RemoveSocket(client->getSocket_fd());
				}
				else
					std::cout << "cast failed" << std::endl;
			}
			else
				std::cout << "Error " << std::endl;
		}
		// fd is ready to write
		else if (this->ep_events->events & EPOLLOUT && f)
		{
			f = 0;
			std::cout << "fd ready to write" << std::endl;
			client = dynamic_cast<HttpHandler*>(this->map[fd]);
			if (client)
			{
				if (client->Write() == 0)
					return this->RemoveSocket(client->getSocket_fd());
			}
		}
	}
}