# pragma once

# include "HttpHandler.hpp"
# include "RequestHandle.hpp"
# include "common.hpp"
# include <fstream>
# include <sys/wait.h>

class Cgi : public RequestHandle
{
	public:
		pid_t pid;
	private:
		int state;
		clock_t start;
		int status;
		std::fstream stream;
		int headersDone;
		std::string outfile;
		std::string script_filename;
		std::string script_name;
	private:
		// member functions
		std::string CheckRessource(data &payload, int &status_code);
		bool GetPath(std::string &res, int &status_code, data &payload);
		bool GetPathdir(std::string &res, int &status_code, data &payload);
		bool checkExtension(std::string &res, std::string &Ext);
		std::string getMethod(data &payload);
		int ChildProcess(data &payload, int &status_code);
		std::string prepareQuerys(data &payload);
		std::string splitHeaders();
	public:
		Cgi(data &payload, int& status_code);
		~Cgi();
		int nextChunk(std::string& chunk, int& code);
};