#pragma once

#include "main.hpp"

class Location;

class ServerConf
{
public:
	std::string root;
	std::string port;
	std::string host;
	std::vector<std::string> Server_names;
	std::map<int, std::string> error_pages;
	std::vector<std::string> index;
	std::map<std::string, Location> locations;

private:
	std::map<std::string, void (ServerConf::*)(std::vector<std::string>)> parseDirective;
	void initMap();
	void validateRoot(const std::vector<std::string> rest);
	void validateHost(const std::vector<std::string> rest);
	void validateListen(const std::vector<std::string> rest);
	void validateServerName(const std::vector<std::string> rest);
	void validateIndex(const std::vector<std::string> rest);
	void validateErrors(const std::vector<std::string> rest);

public:
	ServerConf();
	void validateEverything(const std::vector<ServerConf>& servers);
	void validateDirective(const std::string &token);
	void Init();
	~ServerConf();
};
