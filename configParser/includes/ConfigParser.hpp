#pragma once

#include "../../Utils/Utils.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include "../../Server.hpp"


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
		void DoStuff();
	public:
		ConfigParser();
		ConfigParser(const ConfigParser& other);
		ConfigParser& operator=(const ConfigParser&other);
		void Init(const std::string &filename);
		~ConfigParser();
};
