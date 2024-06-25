#include "includes/ServerConf.hpp"

ServerConf::ServerConf()
{
	this->initMap();
	this->root = "";
	this->host = "";
	this->port = "";
	this->max_body_size = 107374182400;
	this->socket_fd = -1;
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
	this->parseDirective.insert(std::make_pair("client_max_body_size", &ServerConf::validateMaxBody));
}

void ServerConf::validateRoot(std::vector<std::string> rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("multiple/empty roots");
	if (!this->root.empty())
		throw std::runtime_error("root already defined");
	this->root = rest[0];
	if (this->root[root.length() - 1] == '/')
		this->root.substr(0, root.length() - 1);
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

void ServerConf::validateMaxBody(const std::vector<std::string> rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("multiple/empty max client body size");
	// parse exampe 10m
	// check if all numerics except for the last char
	std::string input = rest[0];
	char unit;
	input[input.length() - 1] = std::tolower(input[input.length() - 1]);
	if (input[input.length() - 1] != 'm' && input[input.length() - 1] != 'k'
		&& input[input.length() - 1] != 'g' && !std::isdigit(input[input.length() - 1]))
		throw std::runtime_error("client max body size : Invalid unit");
	if (!std::isdigit(input[input.length() - 1]))
		unit = input[input.length() - 1];
	size_t i = 0;
	unsigned long long res = 0;
	while (i < input.length() - 1)
	{
		if (!std::isdigit(input[i]))
			throw std::runtime_error("client max body size : not a number");
		if (res > (ULLONG_MAX - (input[i] - 48)) / 10)
			throw std::runtime_error("client max body size : number too large");
		res = res * 10 + (input[i] - 48);
		i++;
	}

	const unsigned long long  KILOBYTE = 1024;
    const unsigned long long  MEGABYTE = 1024 * KILOBYTE;
    const unsigned long long  GIGABYTE = 1024 * MEGABYTE;

	unsigned long long maxSizeBytes = 100 * GIGABYTE; // Maximum size in bytes (100G)
    unsigned long long sizeInBytes = 0;
	switch (unit)
	{
        case 'k':
            if (res > maxSizeBytes / KILOBYTE)
                throw std::runtime_error("client max body size: exceeds 100G");
            sizeInBytes = res * KILOBYTE;
            break;
        case 'm':
            if (res > maxSizeBytes / MEGABYTE)
                throw std::runtime_error("client max body size: exceeds 100G");
            sizeInBytes = res * MEGABYTE;
            break;
        case 'g':
            if (res > maxSizeBytes / GIGABYTE)
                throw std::runtime_error("client max body size: exceeds 100G");
            sizeInBytes = res * GIGABYTE;
            break;
        default: break;
    }
	this->max_body_size = sizeInBytes;
}

ServerConf::~ServerConf() {}