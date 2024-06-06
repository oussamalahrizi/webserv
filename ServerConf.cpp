#include "includes/main.hpp"

ServerConf::ServerConf()
{
	this->initMap();
	this->root = "";
	this->host = "";
	this->port = "";
}

void ServerConf::validateDirective(const std::string &token)
{
	std::vector<std::string> splited = Utils::SplitByEach(token, " \t");
	std::map<std::string, void (ServerConf::*)(std::vector<std::string>)>::iterator it = this->parseDirective.find(splited[0]);
	if (it == this->parseDirective.end())
		throw std::runtime_error("unknown directive for ServerConf " + splited[0]);
	(this->*(it->second))(std::vector<std::string>(splited.begin() + 1, splited.end()));
}

void ServerConf::Init()
{
}

void ServerConf::initMap()
{
	this->parseDirective.insert(std::make_pair("listen", &ServerConf::validateListen));
	this->parseDirective.insert(std::make_pair("root", &ServerConf::validateRoot));
	this->parseDirective.insert(std::make_pair("index", &ServerConf::validateIndex));
	this->parseDirective.insert(std::make_pair("host", &ServerConf::validateHost));
	this->parseDirective.insert(std::make_pair("server_name", &ServerConf::validateServerName));
	this->parseDirective.insert(std::make_pair("error_page", &ServerConf::validateErrors));
}

void ServerConf::validateRoot(std::vector<std::string> rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("multiple/empty roots");
	if (!this->root.empty())
		throw std::runtime_error("root already defined");
	this->root = rest[0];
}

void ServerConf::validateHost(std::vector<std::string> rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("multiple/empty host");
	if (!this->host.empty())
		throw std::runtime_error("host already defined");
	this->host = rest[0];
}

void ServerConf::validateListen(std::vector<std::string> rest)
{
	if (rest.size() > 1 || !rest.size())
		throw std::runtime_error("multiple/empty listen");
	if (!this->port.empty())
		throw std::runtime_error("listen already defined");
	if (rest[0].size() < 3)
		throw std::runtime_error("port must be at least 3 digits");
	this->port = rest[0];
}

void ServerConf::validateServerName(std::vector<std::string> rest)
{
	if (!rest.size())
		throw std::runtime_error("Server_name list empty");
	if (this->Server_names.size())
		throw std::runtime_error("mutiple Server_name directive");
	for (size_t i = 0; i < rest.size(); i++)
		this->Server_names.push_back(rest[i]);
}

void ServerConf::validateIndex(std::vector<std::string> rest)
{
	if (!rest.size())
		throw std::runtime_error("index list empty");
	if (this->index.size())
		throw std::runtime_error("multiple index directive");
	for (size_t i = 0; i < rest.size(); i++)
		this->index.push_back(rest[i]);
}

void ServerConf::validateErrors(std::vector<std::string> rest)
{
	if (rest.size() != 2)
		throw std::runtime_error("wrong usage of error page list");
	if (!Utils::CheckNumeric(rest[0], 3))
		throw std::runtime_error("invalid code value for error page");
	this->error_pages[atoi(rest[0].c_str())] = rest[1];
}

void ServerConf::validateEverything(const std::vector<ServerConf>& servers)
{
	if (this->root.empty() || this->host.empty() || this->port.empty())
		throw std::runtime_error("root/host/port missing from ServerConf");
	for(size_t i = 0; i < servers.size(); i++)
	{
		if (servers[i].host == this->host && servers[i].port == this->port)
		{
			const std::vector<std::string > &serverNames = servers[i].Server_names;
			for (size_t j = 0; j < serverNames.size(); j++)
			{
				if (std::find(this->Server_names.begin(),
					this->Server_names.end(), serverNames[j]) != this->Server_names.end())
					throw std::runtime_error("duplicate server name in same host and port");
			}	
		}
	}
	std::map<std::string, Location>::iterator it = this->locations.begin();
	while (it != this->locations.end())
	{
		Location::setInfos(&it->second);
		it++;
	}
}

ServerConf::~ServerConf() {}