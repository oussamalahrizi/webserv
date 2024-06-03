#include "includes/main.hpp"

Server::Server(std::vector<ServerConf>& confs) : confs(confs)
{
    MakeSocket();
}

Server::~Server() {}

void Server::MakeSocket()
{
    int fd;
    size_t i = 0;
    while (i < confs.size())
    {
        if (Utils::findServer(this->hosts, confs[i]))
        {
            i++;
            continue;
        }
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        if (getaddrinfo(confs[i].host.c_str(), confs[i].port.c_str(), &hints, &bind_address))
            std::runtime_error("getaddinfo failed");
        fd = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
        if (fd < 0)
            std::runtime_error("socket failed");
        int resure = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&resure, sizeof(resure)))
            std::runtime_error("setsockopt failed");
        if (bind(fd, bind_address->ai_addr, bind_address->ai_addrlen))
            std::runtime_error("bind failed");
        free(bind_address);
        this->server_fds.push_back(fd);
        this->hosts[confs[i].host] = confs[i].port;
        i++;
    }
}


// listen on RUN

void Server::Start()
{
    Reactor reactor;
    for (size_t i = 0; i < this->server_fds.size(); i++)
    {
        if (listen(server_fds[i], 100) < 0)
            std::runtime_error("listen failed");
        reactor.AddSocket(server_fds[i], new AcceptHandler(server_fds[i], confs));
    }
    reactor.EventPool();
}