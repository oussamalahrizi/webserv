#include "includes/common.hpp"
#include "includes/ServerConf.hpp"
#include "includes/HttpHandler.hpp"

int checkHeaderEnd(std::string& request, size_t &index)
{
    if ((index = request.find(DCRLF)) != std::string::npos)
        return (1);
    return (0);
}

std::map<std::string, std::string> extractHeaders(std::string request)
{
    std::map<std::string, std::string> headers;
    std::vector<std::string> lines = Utils::Split(request, CRLF);
    size_t i = 1, index;
    std::string key, value;
    while (i < lines.size())
    {
        index = lines[i].find(": ");
        if (index == std::string::npos)
            throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
        key = Utils::Trim(lines[i].substr(0, index));
        value = Utils::Trim(lines[i].substr(index + 2));
        headers[key] = value;
        i++;
    }
    if (headers.find("Host") == headers.end())
        throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
    return (headers);
}

int check_protocol(std::string line)
{
    std::vector<std::string> splited = Utils::SplitByEach(line, " ");
    if (splited[2] != "HTTP/1.1")
        return (0);
    return (1);
}

Method getRequestType(std::string line)
{
    std::vector<std::string> splited = Utils::SplitByEach(line, " ");
    if (splited[0] == "GET")
        return (GET);
    if (splited[0] == "POST")
        return (POST);
    if (splited[0] == "DELETE")
        return (DELETE);
    return (OTHER);
}

void ValidateTransfer(std::map<std::string, std::string>& headers, Method type)
{
    // ignore this check if the method is not post
    if (type != POST)
        return;
    /*
        if =>
        Transfer-Encoding not exist
        Content-Length not exist
        The method is Post
    */
    std::map<std::string, std::string>::iterator it = headers.find("Transfer-Encoding");
    if (it == headers.end())
    {
        it = headers.find("Content-Length");
        if (it == headers.end())
            throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
    }
    /*
        if => Transfer-Encoding header
        exist and is different to “chunked”
    */
    else if (it->second != "chunked")
        throw HttpException(http_codes.find(501)->first, http_codes.find(501)->second);
}

void checkAllowedCHars(std::string& uri)
{
    std::string allow = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    size_t i = 0;
    while (i < uri.length())
    {
        if (allow.find(uri[i]) == std::string::npos)
            throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
        i++;
    }
}

void check_uri_path(std::string& uri, ServerConf& handler)
{
    std::vector<std::string> stack;
    std::stringstream ss(uri);
    std::string token;
    std::string resolvedPath;

    while (std::getline(ss, token, '/'))
    {
        if (token == "" || token == ".")
            continue;
        else if (token == "..")
        {
            if (stack.empty())
            {
                resolvedPath = "invalid";
                // The path goes outside the dir;
                break;
            }
            stack.pop_back();
        }
        else
            stack.push_back(token);
    }
    
    if (resolvedPath == "invalid")
        throw HttpException(http_codes.find(403)->first, http_codes.find(403)->second);
    resolvedPath = "/";
    for (size_t i = 0; i < stack.size(); ++i)
    {
        if (i != 0)
            resolvedPath += "/";
        resolvedPath += stack[i];
    }
    uri = handler.root + resolvedPath;
}

ServerConf getServerHandler(std::vector<ServerConf>& confs, std::string& host, int socket_fd)
{

    std::string hostname;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
	size_t index = host.find(":");
    std::stringstream ss;
	hostname = host.substr(0, index);
    // get server info by its socket
    if (getsockname(socket_fd, (struct sockaddr *)&sin, &len) == -1)
        throw HttpException(http_codes.find(500)->first, http_codes.find(500)->second);
    int port_nbr = ntohs(sin.sin_port);
    ss << port_nbr;
	for (size_t i = 0; i < confs.size(); i++)
	{
        // see if the port of the socker matches any server config port
		if (confs[i].port == ss.str())
		{
			std::vector<std::string> server_names = confs[i].Server_names;
			for (size_t j = 0; j < server_names.size(); j++)
			{
				if (server_names[j] == hostname)
					return (confs[i]);
			}
		}
	}
    // return the default config based on the socket port
	for (size_t i = 0; i < confs.size(); i++)
	{
		if (confs[i].port == ss.str())
			return(confs[i]);
	}
    std::cerr << "throwing: server handler not found" << std::endl;
    throw HttpException(http_codes.find(500)->first, http_codes.find(500)->second);
	return (confs[0]);
}


void is_req_well_formed(data &result, std::vector<ServerConf>& confs, int socket_fd)
{
    // std::cout << "transfer " << std::endl;
    ValidateTransfer(result.headers, result.type);
    // std::cout << "uri length " << std::endl;

    if (result.uri.length() > 2048)
        throw HttpException(http_codes.find(414)->first, http_codes.find(414)->second);
    // std::cout << "uri slash " << std::endl;
    if (result.uri[0] != '/')
        throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
    // std::cout << "uri chars " << std::endl;
    checkAllowedCHars(result.uri);
    std::cout << "server handler " << std::endl;
    result.handler = getServerHandler(confs, result.headers.find("Host")->second, socket_fd);
    std::cout << "server root : " << result.handler.root << std::endl;
    check_uri_path(result.uri, result.handler);
}

data Parse(std::string request, std::vector<ServerConf> &servers, int socket_fd)
{
    data result;
    size_t header_end = -1;
    if (!checkHeaderEnd(request, header_end))
        throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
    if (!check_protocol(request.substr(0, request.find_first_of(CRLF))))
        throw HttpException(http_codes.find(505)->first, http_codes.find(505)->second);
    Method type = getRequestType(request.substr(0, request.find_first_of(CRLF)));
    if (type == OTHER)
        throw HttpException(http_codes.find(501)->first, http_codes.find(501)->second);
    result.headers = extractHeaders(request.substr(0, header_end));
    std::vector<std::string> lines = Utils::SplitByEach(request.substr(0, request.find_first_of(CRLF)), " \t");
    result.uri = lines[1];
    is_req_well_formed(result, servers, socket_fd);
    return result;
}