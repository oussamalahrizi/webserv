# pragma once

# include "HttpHandler.hpp"
# include "RequestHandle.hpp"
# include "common.hpp"
# include <fstream>
# include <sys/wait.h>

class Cgi : public RequestHandle
{
	private:
		pid_t pid;
		int state;
		clock_t start;
		int status;
		unsigned long long contentlenght;
		std::fstream stream;
		std::string newfile;
		std::ofstream newstream;
		int headersDone;
		std::string outfile;
		int resline;
		std::string script_filename;
		std::string script_name;
		std::map<std::string, std::string> headers;
	private:
		// member functions
		std::string CheckRessource(data &payload, int &status_code);
		bool GetPath(std::string &res, int &status_code, data &payload);
		bool GetPathdir(std::string &res, int &status_code, data &payload);
		int checkExtension(std::string &res, std::string &Ext);
		std::string getMethod(data &payload);
		int ChildProcess(data &payload, int &status_code);
		std::string prepareQuerys(data &payload);
		std::string splitHeaders();
		void checkResponse(int &status_code);
		void checkHeaders(std::string &res);
		void sendHeaders();
		int ReadBody(std::string &body);
	public:
		Cgi(data &payload, int& status_code);
		~Cgi();
		int nextChunk(std::string& chunk, int& code);
};

std::string decode_uri(const std::string &uri);
