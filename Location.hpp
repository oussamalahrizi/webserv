#pragma once

#include "main.hpp"

class Server;


class Location
{
	public:
		std::string path;
		std::string root;
		std::string redirect;
		std::map<int, std::string> error_pages;
		std::vector<std::string> index;
		std::vector<Method> methods;
		bool autoindex;
		std::vector<Location> nestedLocations;
	public:
		Location();
		Location(std::string nestedPath);
		~Location();
};
