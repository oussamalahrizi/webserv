#pragma once

#include "common.hpp"
#include "HttpHandler.hpp"
#include "AcceptHandler.hpp"


class Reactor
{
	private:
		std::map<int, EventHandler *> map;
		int epoll_fd;
		struct epoll_event *ep_events;
		struct epoll_event ep_event;

	public:
		Reactor();
		void AddSocket(int socket_fd, EventHandler *event);
		void RemoveSocket(int socket_fd);
		void EventPool();
		void Manage(int event_count);
		int isServer(int fd, uint32_t event);
		~Reactor();
};
