
# include "../includes/CgiHandler.hpp"
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/ucontext.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

Cgi::Cgi(data &payload, int& status_code)
{
	state = 0;
	headersDone = 0;
	pid = -1;
	if ((script_filename = CheckRessource(payload, status_code)) == "")
	{
		std::cout << "SCRIPT FILE NOT FOUND" << status_code <<std::endl;
		std::cout << "type : " << payload.type << std::endl;	
		return ;
	}
	size_t index = script_filename.find(payload.loc.root) + payload.loc.root.length();
	script_name = script_filename.substr(index);
	script_name = payload.loc.path + script_name;
	std::cout << "script path : " << script_filename << std::endl;
	if (ChildProcess(payload, status_code) == -1)
		status_code = 500;
}


std::string getType(const std::string& filename)
{
	size_t pos = filename.find_last_of(".");
	if (pos == std::string::npos)
		return ("application/octet-stream");
	return (mimetype.find(filename.substr(pos))->second);
}


std::string replace_char(const std::string& str)
{
	size_t i = 0;
	std::string res = "";
	while (i < str.length())
	{
		if (str[i] == '-')
			res += '_';
		else
			res += std::toupper(str[i]);
		i++;
	}
	return (res);
}

int Cgi::ChildProcess(data &payload, int &status_code)
{
	start = clock();
	outfile = UUID::generate();
	int tmpfile = open(outfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (tmpfile == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		status_code = 500;
		return (-1);
	}
	if (pid == 0)
	{
		std::vector<std::string> tmp;
		std::string queries = prepareQuerys(payload);
		tmp.push_back("REQUEST_METHOD=" + getMethod(payload));
		tmp.push_back("SCRIPT_FILENAME=" + script_filename);
		tmp.push_back("SCRIPT_NAME=" + script_name);
		tmp.push_back("QUERY_STRING=" + queries);
		tmp.push_back("GATEWAY_INTERFACE=CGI/1.1");
		tmp.push_back("SERVER_PROTOCOL=HTTP/1.1");
		tmp.push_back("SERVER_SOFTWARE=CTRL+ALT+DEFEAT");
		tmp.push_back("REMOTE_ADDRESS=" + payload.info.client_ip);
		tmp.push_back("SERVER_NAME=" + payload.server_name);
		tmp.push_back("SERVER_PORT=" + payload.handler.port);
		tmp.push_back("REDIRECT_STATUS=CGI");
		tmp.push_back("PATH_INFO=");
		tmp.push_back("HTTP_HOST=" + payload.server_name + ":" + payload.handler.port);
		std::string uri = payload.ressource;
		if (payload.url_params.size())
			uri += "?" + queries;
		tmp.push_back("REQUEST_URI=" + uri);
		if (payload.headers.find("Cookie") != payload.headers.end())
			tmp.push_back("HTTP_COOKIE=" + payload.headers.find("Cookie")->second);
		// adding headers 
		std::map<std::string, std::string>::iterator it = payload.headers.begin();
		while (it != payload.headers.end())
		{
			if (it->first != "Content-Type" && it->first != "Content-Length")
				tmp.push_back("HTTP_" + replace_char(it->first) + "=" + replace_char(it->second));
			it++;
		}
		int bodyFile = -1;
		if (payload.type == POST)
		{
			struct stat filestat;
			if (stat(payload.tempfile_name.c_str(), &filestat) == -1)
			{
				close(tmpfile);
				exit(-1);
			}
			std::stringstream ss;
			ss << filestat.st_size;
		 	tmp.push_back("CONTENT_LENGTH=" + ss.str());
			std::cout << "cl : " + ss.str() << std::endl;
			std::cout << "ct : " << getType(payload.tempfile_name) << std::endl;
		 	tmp.push_back("CONTENT_TYPE=" + getType(payload.tempfile_name));
			bodyFile = open(payload.tempfile_name.c_str(), O_RDONLY);
			if (bodyFile < 0)
			{
				close(tmpfile);
				exit(-1);
			}
			if (dup2(bodyFile, STDIN_FILENO) < 0)
			{
				close(tmpfile);
				close(bodyFile);
				exit(-1);
			}
			std::cout << "redirect in to body " << std::endl;
		}
		if (dup2(tmpfile, STDOUT_FILENO) < 0)
		{
			close(tmpfile);
			close(bodyFile);
			exit(-1);
		}
		// execute
		std::vector<char *> env(tmp.size() + 1);
		for(size_t i = 0; i < tmp.size(); i++)
			env[i] = (char *)(tmp[i].c_str());
		char *cmdargs[3];
		cmdargs[0] = (char *)payload.loc.cgi_path.c_str();
		if (!access(payload.loc.cgi_path.c_str(), F_OK))
		{
			if (access(payload.loc.cgi_path.c_str(), X_OK))
				exit(-1);
		}
		else
		{
			std::cerr << "file not found " << std::endl;
			exit(-1);
		}
		cmdargs[1] = (char *)script_filename.c_str();
		cmdargs[2] = NULL;
		execve(cmdargs[0], cmdargs, env.data());
		exit(-1);
	}
	close(tmpfile);
	return(0);
}

std::string Cgi::prepareQuerys(data &payload)
{
	std::string res = "";
	if (!payload.url_params.size())
		return  res;
	std::map<std::string, std::string>::iterator it = payload.url_params.begin();
	while (it != payload.url_params.end())
	{
		res += (it->first + "=" + it->second);
		it++;
		if (it != payload.url_params.end())
			res += "&";
	}
	return (res);
}

std::string Cgi::getMethod(data &payload)
{
	if (payload.type == DELETE)
		return ("DELETE");
	else if (payload.type == POST)
		return("POST");
	return("GET");
}

std::string Cgi::CheckRessource(data &payload, int &status_code)
{
	std::string res = payload.ressource;
    std::string newLocation;
    newLocation = res.substr(payload.loc.path.length());
    if (newLocation.empty())
        newLocation = "/";
    res = payload.loc.root + newLocation;
	if (res.at(res.size() - 1) == '/')
	{
		if (GetPathdir(res, status_code, payload))
			return res;
		return "";
	}
	else
	{
		if (GetPath(res, status_code, payload))
			return res;
		return "";
	}
}

int Cgi::checkExtension(std::string &path, std::string &Ext)
{
	std::string tmp;
	size_t index = path.find_last_of(".");
	if (index == std::string::npos)
		return (1);
	tmp = path.substr(index + 1);
	if (tmp == Ext)
		return (0);
	return(1);
}

bool Cgi::GetPathdir(std::string &res, int &status_code, data &payload)
{
	struct stat path_stat;
    std::string tmppath;
    res.erase(res.length() - 1);
    if (stat(res.c_str(), &path_stat) == -1)
    {
        if (errno == ENOENT)
        {
        	status_code = 404;
        	return false;
        }
        status_code = 500;
        return false;
    }
    if (S_ISDIR(path_stat.st_mode))
    {
		for (size_t i = 0; i < payload.handler.index.size(); i++)
		{
			tmppath = res + "/" + payload.handler.index[i];
			if (!checkExtension(tmppath, payload.loc.cgi_ext))
			{
				if (!access(tmppath.c_str(), F_OK))
				{
					res = tmppath;
					return true;
				}
			}
			tmppath.clear();
		}
    }
    status_code = 404;
    return false;
}

bool Cgi::GetPath(std::string &res, int &status_code, data &payload)
{
	struct stat path_stat;
    std::string tmppath;
    if (stat(res.c_str(), &path_stat) == -1)
    {
        if (errno == ENOENT)
        {
        	status_code = 404;
        	return false;
        }
        status_code = 500;
        return false;
    }
    if (S_ISREG(path_stat.st_mode))
    {
		if (!checkExtension(res, payload.loc.cgi_ext))
		{
			if (!access(res.c_str(), F_OK))
				return (true);
		}
		else
		{
			status_code = 404;
			return (false);
		}
    }
    if (S_ISDIR(path_stat.st_mode))
    {
		for (size_t i = 0; i < payload.handler.index.size(); i++)
		{
			tmppath = res + "/" + payload.handler.index[i];
			if (!checkExtension(tmppath, payload.loc.cgi_ext))
			{
				if (!access(tmppath.c_str(), F_OK))
				{
					res = tmppath;
					return true;
				}
			}
			tmppath.clear();
		}
    }
    status_code = 404;
    return (false);
}

Cgi::~Cgi()
{
	if (stream.is_open())
		stream.close();
	unlink(outfile.c_str());
	if (pid != -1)
	{
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
	}
}

std::string Cgi::splitHeaders()
{
	std::string line;
	std::string del = "\r\n\r\n";
	std::string content;
	std::string buffer;
	while (std::getline(stream, line))
	{
        buffer += line + "\n";  // Adding '\n' since std::getline discards the delimiter
        content += line + "\n"; // Same here

        if (buffer.size() >= 4 && buffer.substr(buffer.size() - 4) == "\r\n\r\n") {
            break;
        } else if (buffer.size() > 4) {
            buffer.erase(0, buffer.size() - 4);  // Keep the last 4 characters for checking
        }
    }
	stream.close();
    return content;
}

int Cgi::nextChunk(std::string& chunk, int& code)
{
	if (state == 0)
	{
		int value = waitpid(pid, &status, WNOHANG);
		if (value == 0)
		{
			if (clock() - start > 3 * CLOCKS_PER_SEC)
			{
				std::cout << "killing process : " << pid << std::endl;
				std::cout << kill(pid, SIGKILL) << std::endl;
				waitpid(pid, NULL, 0);
				code = 504;
				return(1);
			}
			return 0;
		}
		else if (value > 0)
		{
			std::cout << "process finished" << std::endl;
			if (WIFEXITED(status))
			{
				if (WEXITSTATUS(status) == 255)
					code = 502;
				state = 1;
				return 0;
			}
			code = 502;
			return (1);
		}
		else if (value < 0)
		{
			code = 502;
			std::cout << "waitpid failed " << pid << std::endl;
			return (1);
		}
	}
	else
	{
		if (!headersDone)
			stream.open(outfile.c_str(), std::ios::in);
		char buffer[READ_SIZE];
		stream.read(buffer, READ_SIZE);
		size_t readed = stream.gcount();
		chunk = std::string(buffer, readed);
		if (!headersDone && chunk.find("HTTP/1.1") == std::string::npos)
		{
			std::cout << headersDone << std::endl;
			std::string temp = "HTTP/1.1 " + http_codes[code] + "\r\n";
			stream.seekg(-temp.length() , std::ios::cur);
			chunk = temp + chunk.substr(temp.length());
		}
		headersDone = 1;
		if (stream.eof())
		{
			stream.close();
			return (1);
		}
		return (0);
	  }
	return (0);
}


// TODO:
// get ressource file 
// directory: check index
// match extension
// prepare env variables
// headers
// open temp file: generate name
// fork, dup output
// execve
// waitpid
// 
// 
// HTTP/1.1 200 OK\r\n
// Content-Type: text/html\r\n
// Content-Length: 100\r\n
// Connection: close\r\n
// \r\n
// asdasdad