#pragma once


#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include "../../Utils/Utils.hpp"
#include <stdexcept>

class ConfigParser
{
	private:
		std::map<std::string, std::string> conf;
		std::ifstream file;
	public:
		ConfigParser();
		ConfigParser(const std::string& filename);
		void DoStuff();
		void CheckKeys(std::string& string, const std::string& check);
		void fill(std::string content);
		void setKeyword(std::string key, std::string value);
		~ConfigParser();
};
