#pragma once

#include "RequestHandle.hpp"
#include "HttpHandler.hpp"
#include "Autoindex.hpp"


class GetRequest : public RequestHandle
{
	private:
		struct stat filestat;
		int fd;
		data payload;
		std::string file;
		std::string ressource;
		int error;
		std::map<std::string, std::string> headers;
		int headers_done;
		Autoindex* auto_index;
	private:
		void handleServeRoot();
		void setError(int code);
		void get_index(std::string& name, const std::string& root);
		void setHeaders(const std::string& key, const std::string &value);
		void handleRessource(int autoindex, const std::string& root, const std::string& res);
	public:
		GetRequest(data& payload, int& status_cod);
		int nextChunk(std::string& chunk, int& code);
		~GetRequest();
};
