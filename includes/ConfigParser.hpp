#pragma once

#include "common.hpp"
#include "ServerConf.hpp"

class ConfigParser
{
	private:
		std::ifstream file;
		std::vector<std::string> tokens;
		std::map<std::string, std::string> directives;
		size_t token_index;
		void Tokenize(std::string &content, size_t &i);
		void CheckServerConf();
		std::string &nextToken();
		void ValidateDirectives(std::vector<ServerConf> &ServerConfs);
		void LocationLexer(std::string &current, ServerConf *ServerConf, Location *parent);
		void DoStuff(std::vector<ServerConf> &ServerConfs);

	public:
		ConfigParser();
		ConfigParser(const ConfigParser &other);
		ConfigParser &operator=(const ConfigParser &other);
		void Init(const std::string &filename, std::vector<ServerConf> &ServerConfs);
		~ConfigParser();
};
