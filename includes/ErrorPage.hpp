
#pragma once

#include "common.hpp"
#include "HttpHandler.hpp"

class ErrorPage
{
	private:
		std::string filename;
		struct stat filestat;
		int headers_done;
		std::fstream stream;
		int gen;
		int code;
	private:
		std::string generate(int code) const;
	public:
		ErrorPage(data& payload, int status_code);
		int next_chunk(std::string& chunk);
		~ErrorPage();
};

