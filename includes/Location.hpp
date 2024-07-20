#pragma once

#include "common.hpp"
#include "ServerConf.hpp"

class Location
{
	private:
		int first;
		ServerConf *conf;
		static void initMap();
		static std::map<std::string, void (Location::*)(const std::vector<std::string> &)>
			Directives;
		void ValidatePath(const std::vector<std::string> &rest);
		void validateRoot(const std::vector<std::string> &rest);
		void validateRedirect(const std::vector<std::string> &rest);
		void validateErrors(const std::vector<std::string> &rest);
		void validateMethods(const std::vector<std::string> &rest);
		void validateAutoindex(const std::vector<std::string> &rest);
		void validateUpload(const std::vector<std::string> &rest);
		void validateCGI(const std::vector<std::string> &rest);
		void validateCgiPath(const std::vector<std::string> &rest);

	public:
		int met;
		std::string cgi_ext;
		std::string cgi_path;
		std::string path;
		std::string root;
		std::string redirect;
		std::string upload;
		int up;

		int redirect_code;
		std::map<int, std::string> error_pages;
		std::vector<Method> methods;
		bool autoindex;
		void ValidateDirective(const std::string &token);
		void ValidateEverything();
		Location();

	public:
		Location(ServerConf &conf);
		// Location& operator=(const Location& other);
		~Location();
};
