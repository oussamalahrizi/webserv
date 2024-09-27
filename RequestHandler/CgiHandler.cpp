
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
#include <ostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/ucontext.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

Cgi::Cgi(data &payload, int& status_code, int get) : payload(payload)
{
	state = 0;
	headersDone = 0;
	pid = -1;
	if ((script_filename = CheckRessource(payload, status_code)) == "")
	{
		std::cout << "SCRIPT FILE NOT FOUND" << status_code <<std::endl;
		// std::cout << "type : " << payload.type << std::endl;
		return ;
	}
	size_t index = script_filename.find(payload.loc.root) + payload.loc.root.length();
	script_name = script_filename.substr(index);
	script_name = payload.loc.path + script_name;
	std::cout << "script path : " << script_filename << std::endl;
	if (ChildProcess(payload, status_code) == -1)
	{
		status_code = 500;
		return ;
	}
	
}

Cgi::~Cgi()
{
	if (stream.is_open())
		stream.close();
	if (newstream.is_open())
		newstream.close();
	if (!access(outfile.c_str(), F_OK) && unlink(outfile.c_str()) == -1)
	{
		std::cerr << "failed to delete outfile cgi : " << errno << std::endl;
		while (1);
	}
	if (!access(newfile.c_str(), F_OK) && unlink(newfile.c_str()) == -1)
	{
		std::cerr << "failed to delete newfile cgi" << errno <<  std::endl;
		while (1);
	}
	if (pid != -1)
	{
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
	}
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
	outfile = payload.loc.root + "/" + UUID::generate() + "cgi_outfile";
	
	pid = fork();
	if (pid == -1)
	{
		status_code = 500;
		return (-1);
	}
	if (pid == 0)
	{
		int tmpfile = open(outfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
		if (tmpfile == -1)
			exit(-1);
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
				std::cerr << "body file child" << std::endl;
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
					std::cerr << "body file child 2" << std::endl;
				close(tmpfile);
				exit(-1);
			}
			if (dup2(bodyFile, STDIN_FILENO) < 0)
			{
					std::cerr << "body file child 3" << std::endl;
				close(tmpfile);
				close(bodyFile);
				exit(-1);
			}
			std::cout << "redirect in to body " << std::endl;
		}
		if (dup2(tmpfile, STDOUT_FILENO) < 0)
		{
				std::cerr << "body file child 4" << std::endl;
			close(tmpfile);
			close(bodyFile);
			exit(-1);
		}
		// execute
		// env variables
		std::cerr << "env variables :" << std::endl;
		std::vector<char *> env(tmp.size() + 1);
		for(size_t i = 0; i < tmp.size(); i++)
		{
			env[i] = (char *)(tmp[i].c_str());
			std::cerr << env[i] << std::endl;
		}
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
	// close(tmpfile);
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
    if (payload.loc.path != "/")
    	newLocation = res.substr(payload.loc.path.length());
    if (newLocation.empty())
        newLocation = "/";
    // std::cout << "new location : " << newLocation << std::endl;
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

// static int isError(int code)
// {
// 	if (code >= 400 && code <= 511)
// 		return (1);
// 	return (0);
// }

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
				{
					code = 502;
					return 1;
				}
				checkResponse(code);
				if (code == 502)
				{
					std::cout << "check response" << std::endl;
					return 1;
				}
				else // assuming 200 cuz we dont wanna show server error pages
					code = 200;
				std::cout << "process status code " << code << std::endl;
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
		// if (isError(status_code))
		// 	return 1;
		if (!headersDone)
		{
			stream.open(newfile.c_str(), std::ios::in);
			headersDone = 1;
		}
		char buffer[READ_SIZE];
		stream.read(buffer, READ_SIZE);
		size_t readed = stream.gcount();
		chunk = std::string(buffer, readed);
		if (stream.eof())
		{
			stream.close();
			return (1);
		}
		return (0);
	  }
	return (0);
}

static std::multimap<std::string, std::string> extractHeaders(std::string request)
{
	std::multimap<std::string, std::string> headers;
	std::vector<std::string> lines = Utils::Split(request, CRLF);
	size_t i = 0, index;
	std::string key, value;
	std::cout << "header size : " << lines.size() << std::endl;
	while (i < lines.size())
	{
		std::cout << "<" + lines[i] + ">" << std::endl;
		index = lines[i].find(": ");
		if (index == std::string::npos)
		{
			std::cout << ": headers" << std::endl;
			throw HttpException(502);
		}
		key = Utils::Trim(lines[i].substr(0, index));
		value = Utils::Trim(lines[i].substr(index + 2));
		headers.insert(std::make_pair(key, value));
		i++;
	}
	std::cout << "HEADERS SIZE AND i SIZE " << headers.size() << " " << i << std::endl;
	// while (1);
	return (headers);
}


void Cgi::find_status_line(const std::string& headers)
{
	resline = 0;
	size_t pos = headers.find(CRLF);
	if (pos == std::string::npos)
	{
		std::cout << headers << std::endl;
		std::cerr << "crlf for response line not there" << std::endl;
		throw HttpException(502);
	}
	std::string line = headers.substr(0, pos);
	if (!strncmp("HTTP/1.1 ", line.c_str(), 9))
	{
		resline = 1;
		// making sure the status code is correct
		std::string status = line.substr(9);
		map_it it = http_codes.begin();
		while (it != http_codes.end())
		{
			if (it->second == status)
			{
				newstream << "HTTP/1.1 " + status + CRLF;
				std::stringstream ss(status.substr(0, 3));
				ss >> status_code;
				return;
			}
			it++;
		}
		if (it == http_codes.end())
		{
			std::cerr << "status code not valid" << std::endl;
			throw HttpException(502);
		}
	}
}

void Cgi::parseHeaders(const std::string& headers)
{
	std::string rest;
	// parse headers into the map;
	rest = headers.substr(0, headers.find(DCRLF));
	if (resline)
	{
		// remove first line
		size_t index = rest.find(CRLF);
		if (index != std::string::npos)
			rest = rest.substr(index + 2);
		else
			rest = "";
	}
	if (!rest.empty())
		this->headers = extractHeaders(rest);
	// set transfer strat
	if (this->headers.find("Content-Length") != this->headers.end())
	{
		std::stringstream ss(this->headers.find("Content-Length")->second);
		ss >> contentlenght;
		this->body = 1;
	}
	else if (this->headers.find("Transfer-encoding") != this->headers.end())
		this->body = 1;
	// extract status code from headers in case response line doesnt exist
	if (!resline && this->headers.find("Status") != this->headers.end())
	{
		std::string status_head = this->headers.find("Status")->second;
		map_it v = http_codes.begin();
		while (v != http_codes.end())
		{
			if (v->second == status_head)
				break;
			v++;
		}
		if (v == http_codes.end())
		{
			std::cout << "status header invalid" << std::endl;
			throw HttpException(502);
		}
		std::stringstream ss(status_head.substr(0, 3));
		ss >> status_code;
		newstream << "HTTP/1.1 " + this->headers.find("Status")->second + CRLF;
	}
	else if (!resline)
	{
		std::cout << "HARD CODING STATUS LINE" << std::endl;
		status_code = 200;
		newstream << "HTTP/1.1 200 OK\r\n";
	}
	// set content type if not existant for the client to download instead
	if (this->headers.find("Content-Type") == this->headers.end() && body)
		this->headers.insert(std::make_pair("Content-Type", "application/octet-stream"));
	// close connection / override if the header exist
	this->headers.erase("Connection");
	this->headers.insert(std::make_pair("Connection", "close"));
	map_it_str it1 = this->headers.begin();
	while (it1 != this->headers.end())
	{
		std::cout << "setting header : " << it1->first + ": " + it1->second << std::endl;
		newstream << it1->first << ": " << it1->second + CRLF;
		it1++;
	}
	newstream << CRLF;
}

void Cgi::checkResponse(int &status_code)
{
	this->body = 0;
	std::string all_headers = ""; 
	try
	{
		stream.open(outfile.c_str(), std::ios::in);
		newfile = payload.loc.root + "/" + UUID::generate();
		newstream.open(newfile.c_str());
		if (!newstream.is_open() || !stream.is_open())
		{
			std::cerr << "failed to open file" << std::endl;
			throw HttpException(502);
		}
		char buffer[READ_SIZE];
		stream.read(buffer, READ_SIZE);
		all_headers.append(buffer, stream.gcount());
		size_t end = all_headers.find(DCRLF);
		if (end == std::string::npos)
		{
			std::cerr << "headers end not found" << std::endl;
			throw HttpException(502);
		}
		std::string rest = all_headers.substr(end + 4);
		// try find status line
		find_status_line(all_headers);
		parseHeaders(all_headers);
		// let the client read body without any transfer strat
		// std::cout << "cl ? " << body << std::endl;
		// std::cout << "rest size ? " << rest.size() << std::endl;
		// std::cout << "stream end ? " << stream.eof() << std::endl;
		// if (!body && (rest.size() || !stream.eof()))
		// {
		// 	std::cerr << "there is body without cl" << std::endl;
		// 	// calculate the content length
		// 	add_cl = 1;
		// }
		if (body && (!rest.size() && stream.eof()))
		{
			std::cerr << "there is cl without body" << std::endl;
			throw HttpException(502);
		}
		else
		{
			if (rest.size())
			{
				newstream << rest;
			}
			while (1)
			{
				rest.clear();
				stream.read(buffer, READ_SIZE);
				rest.append(buffer, stream.gcount());
				newstream << rest;
				if (stream.eof())
					break;
			}
		}
		newstream.close();
		stream.close();
		std::cout << "EVERYTHING IS GOOOD!" << std::endl;
	}
	catch (const HttpException& e)
	{
		status_code = 502;
		newstream.close();
		stream.close();
		return;
	}
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