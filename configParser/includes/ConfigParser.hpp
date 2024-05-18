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

typedef struct err_p
{
	std::map<int, std::string> err;
}	err_pages;

typedef struct s_locations
{
	std::string path;
	std::vector<std::string> methods;
	bool autoindex;
	std::string *root;
	std::string *redirect;
	std::map<int, std::string> error_pages;
}	location;

typedef struct s_configs
{
	std::vector<std::string> ports;
	std::string host;
	std::vector<std::string> server_names;
	std::string root;
	std::vector<std::string> index;
	std::vector<location> locations;
	std::map<int, std::string> error_pages;
}	configs;

class ConfigParser
{
	public:
		configs conf;
	private:
		std::ifstream file;
	public:
		ConfigParser();
		ConfigParser(const std::string &filename);
		ConfigParser& operator=(const ConfigParser&other);
		void DoStuff();
		void SplitBySemicolon(std::string content);
		void remove_comments(std::string &input, char c);
		void HandleKeywords(std::vector<std::string> &lines);
		~ConfigParser();
};
