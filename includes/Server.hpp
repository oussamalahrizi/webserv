#pragma once

#include "common.hpp"
#include "ServerConf.hpp"
#include "Reactor.hpp"
#include "Utils.hpp"

class Server
{
    private:
        struct addrinfo hints;
	    struct addrinfo *bind_address;
        std::vector<int> server_fds;
        std::map<std::string, std::string> hosts;
    public:
        Server(std::vector<ServerConf>& confs);
        void MakeSocket();
        std::vector<ServerConf> confs;
        void Start();
        ~Server();
};

