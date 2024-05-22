#pragma once

#include "main.hpp"

class ConfigParser
{
	private:
		std::vector<Server> servers;
		std::ifstream file;
		std::vector<std::string> tokens;
		std::map<std::string, std::string> directives;
		size_t token_index;
		void Tokenize(std::string& content, size_t& i);
		void CheckServer();
		std::string& nextToken();
		void ValidateDirectives();
		void LocationLexer(std::string& current, Server* server, Location* parent);
		void DoStuff();
	public:
		ConfigParser();
		ConfigParser(const ConfigParser& other);
		ConfigParser& operator=(const ConfigParser&other);
		void Init(const std::string &filename);
		~ConfigParser();
};
