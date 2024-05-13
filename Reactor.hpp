#pragma  once

#include <map>
#include <sys/epoll.h>
#include "EventHandlers/AcceptHandler.hpp"
#include "EventHandlers/HttpHandler.hpp"
#include <unistd.h>
#include <iostream>

class Reactor
{
	private:
		std::map<int, EventHandler*> map;
		int epoll_fd;
		struct epoll_event *ep_events;
	public:
		Reactor();
		void AddSocket(int socket_fd, EventHandler* event);
		void RemoveSocket(int socket_fd);
		void EventPool();
		void Manage(int event_count);
		~Reactor();
};
