#pragma once

#include "main.hpp"

class Location
{
	private:
		int first;
		Server *server;
		static std::map<std::string, void (Location::*)(const std::vector<std::string>&)>
			Directives;
		static void initMap();
		void ValidatePath(const std::vector<std::string>& rest);
		void validateRoot(const std::vector<std::string>& rest);
		void validateRedirect(const std::vector<std::string>& rest);
		void validateErrors(const std::vector<std::string>& rest);
		void validateMethods(const std::vector<std::string>& rest);
		void validateAutoindex(const std::vector<std::string>& rest);
	public:
		int met;
		std::string path;
		std::string root;
		std::string redirect;
		std::map<int, std::string> error_pages;
		std::vector<Method> methods;
		bool autoindex;
		std::vector<Location> nestedLocations;
		void ValidateDirective(const std::string& token);
		void ValidateEverything(Location* parent);
		Location();
	public:
		Location(Server& server);
		~Location();
};