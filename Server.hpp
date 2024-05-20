#pragma once

#include "main.hpp"

class Server
{
	public:
		std::string root;
		std::string port;
		std::string host;
		std::vector<std::string> server_names;
		std::map<int, std::string> error_pages;
		std::vector<std::string> index;
	private:
		std::map<std::string, void (Server::*)(const std::vector<std::string>)> parseDirecive;
		void initMap();
		void validateRoot(const std::vector<std::string> rest);
		void validateHost(const std::vector<std::string> rest);
		void validateListen(const std::vector<std::string> rest);
		void validateServerName(const std::vector<std::string> rest);
		void validateIndex(const std::vector<std::string> rest);
		void validateErrors(const std::vector<std::string> rest);
	public:
		Server();
		void validateEverything();
		void validateDirective(const std::string& token);
		void Init();
		~Server();
};

