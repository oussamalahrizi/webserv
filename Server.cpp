#include "includes/main.hpp"


Server::Server()
{
	this->initMap();
	this->root = "";
	this->host = "";
	this->port = "";
}

void Server::validateDirective(const std::string& token)
{
	std::vector<std::string> splited = Utils::SplitByEach(token, " \t");
	std::map<std::string, void (Server::*)(std::vector<std::string>)>
		::iterator it = this->parseDirective.find(splited[0]);
	if (it == this->parseDirective.end())
		throw std::runtime_error("unknown directive for server " + splited[0]);
	(this->*(it->second))(std::vector<std::string>(splited.begin() + 1, splited.end()));
}


void Server::Init()
{
}

void Server::initMap()
{
	this->parseDirective.insert(std::make_pair("listen", &Server::validateListen));	
	this->parseDirective.insert(std::make_pair("root", &Server::validateRoot));
	this->parseDirective.insert(std::make_pair("index", &Server::validateIndex));
	this->parseDirective.insert(std::make_pair("host", &Server::validateHost));
	this->parseDirective.insert(std::make_pair("server_name", &Server::validateServerName));
	this->parseDirective.insert(std::make_pair("error_page", &Server::validateErrors));
}

void Server::validateRoot(std::vector<std::string> rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("multiple/empty roots");
	if (!this->root.empty())
		throw std::runtime_error("root already defined");
	this->root = rest[0];
}

void Server::validateHost(std::vector<std::string> rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("multiple/empty host");
	if (!this->host.empty())
		throw std::runtime_error("host already defined");
	this->host = rest[0];
}

void Server::validateListen(std::vector<std::string> rest)
{
	if (rest.size() > 1 || !rest.size())
		throw std::runtime_error("multiple/empty listen");
	if (!this->port.empty())
		throw std::runtime_error("listen already defined");
	if (rest[0].size() < 3)
		throw std::runtime_error("port must be at least 3 digits");
	this->port = rest[0];
}

void Server::validateServerName(std::vector<std::string> rest)
{
	if (!rest.size())
		throw std::runtime_error("server_name list empty");
	if (this->server_names.size())
		throw std::runtime_error("mutiple server_name directive");
	for (size_t i = 0; i < rest.size(); i++)
		this->server_names.push_back(rest[i]);
}

void Server::validateIndex(std::vector<std::string> rest)
{
	if (!rest.size())
		throw std::runtime_error("index list empty");
	if (this->index.size())
		throw std::runtime_error("multiple index directive");
	for (size_t i = 0; i < rest.size(); i++)
		this->index.push_back(rest[i]);	
}

void Server::validateErrors(std::vector<std::string> rest)
{
	if (rest.size() != 2)
		throw std::runtime_error("wrong usage of error page list");
	if (!Utils::CheckNumeric(rest[0], 3))
		throw std::runtime_error("invalid code value for error page");
	this->error_pages[atoi(rest[0].c_str())] = rest[1];
}

void Server::validateEverything()
{
	if (this->root.empty() || this->host.empty() || this->port.empty())
		throw std::runtime_error("root/host/port missing from server");
	this->server_names.push_back(this->host);
	std::map<std::string, Location>::iterator it = this->locations.begin();
	while (it != this->locations.end())
	{
		Location::setInfos(&it->second);
		it++;
	}
}

Server::~Server() {}