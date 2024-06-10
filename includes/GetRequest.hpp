#pragma once

#include "main.hpp"

class GetRequest
{
	private:
		int fd;
		ServerConf handler;
		std::string ressource;
		std::string accept;
	public:
		GetRequest(ServerConf& handler, const std::string& ressource, const std::string& accept);
		Response* GetRessource();
		~GetRequest();
};
