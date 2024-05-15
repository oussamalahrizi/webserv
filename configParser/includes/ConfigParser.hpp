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
	std::string *root = NULL;
	std::string *redirect = NULL;
	std::vector<err_pages> error_pages;
}	location;

typedef struct s_configs
{
	std::vector<std::string> ports;
	std::string host;
	std::vector<std::string> server_names;
	std::string root;
	std::vector<std::string> index;
	std::vector<location> locations;
	std::vector<err_pages> error_pages;
}	configs;

class ConfigParser
{
	private:
		std::ifstream file;
		configs conf;
	public:
		ConfigParser();
		ConfigParser(const std::string &filename);
		void DoStuff();
		void SplitBySemicolon(std::string content);
		void remove_comments(std::string &input, char c);
		void HandleKeywords(std::vector<std::string> &lines);
		~ConfigParser();
};
