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

void check_uri_path(std::string& uri, std::string& ressource, ServerConf& handler)
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
            // The path goes outside the dir;
            if (stack.empty())
            {
                resolvedPath = "invalid";
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
    int dir = uri[uri.length() - 1] == '/' && uri.length() != 1 ? 1 : 0;
    ressource = handler.root + resolvedPath;
    if (dir)
        ressource += '/';
    std::cout << "Ressource : " << ressource << std::endl;
}

ServerConf getServerHandler(std::vector<ServerConf>& confs, std::string& host, int socket_fd)
{

    std::string hostname, port;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
	size_t index = host.find(":");
    std::stringstream ss;
	hostname = host.substr(0, index);
	port = host.substr(index + 1);
    // get server info by its socket
    if (getsockname(socket_fd, (struct sockaddr *)&sin, &len) == -1)
        throw HttpException(http_codes.find(500)->first, http_codes.find(500)->second);
    int port_nbr = ntohs(sin.sin_port);
    ss << port_nbr;
    if (port != ss.str()) // if asked port by the header host is not the port by socket
    // return the default config based on the socket port
    {
        for (size_t i = 0; i < confs.size(); i++)
        {
            if (confs[i].port == ss.str())
                return(confs[i]);
        }
    }
	for (size_t i = 0; i < confs.size(); i++)
	{
        // see if the port of the socket matches any server config port
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
    // most likely we wont get here
    std::cerr << "throwing: server handler not found" << std::endl;
    throw HttpException(http_codes.find(500)->first, http_codes.find(500)->second);
	return (confs[0]);
}


void is_req_well_formed(data &result, std::vector<ServerConf>& confs, int socket_fd)
{
    ValidateTransfer(result.headers, result.type);
    if (result.uri.length() > 2048)
        throw HttpException(http_codes.find(414)->first, http_codes.find(414)->second);
    if (result.uri[0] != '/')
        throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
    checkAllowedCHars(result.uri);
    result.handler = getServerHandler(confs, result.headers.find("Host")->second, socket_fd);
    check_uri_path(result.uri, result.ressource, result.handler);
}

std::string getLocationByUri(std::map<std::string, Location>& locations, std::string& uri)
{
    std::map<std::string, Location>::iterator it = locations.begin();
    std::map<std::string, Location>::iterator found = locations.end();
    std::cout << "uri : " << uri << std::endl;
    while (it != locations.end())
    {
        if (!uri.compare(0, it->first.size(), it->first))
        {
            if (found == locations.end() || it->first.size() > found->first.size())
                found = it;
        }
        it++;
    }
    if (found != locations.end())
        return (found->first);
    if (locations.find("/") != locations.end())
        return ("/");
    return ("");
}

void validateLocation(std::string loc_name, data result)
{
    Location loc = result.handler.locations.find(loc_name)->second;
    std::cout << "location needed : " << loc_name << std::endl;
    if (std::find(loc.methods.begin(), loc.methods.end(), result.type) == loc.methods.end())
        throw HttpException(http_codes.find(405)->first, http_codes.find(405)->second);
    result.loc = loc;
}



data Parse(std::string request, std::vector<ServerConf> &servers, int socket_fd)
{
    data result;
    size_t header_end = -1;
    std::cout << request << std::endl;
    if (!checkHeaderEnd(request, header_end))
        throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
    if (!check_protocol(request.substr(0, request.find_first_of(CRLF))))
        throw HttpException(http_codes.find(505)->first, http_codes.find(505)->second);
    Method type = getRequestType(request.substr(0, request.find_first_of(CRLF)));
    if (type == OTHER)
        throw HttpException(http_codes.find(501)->first, http_codes.find(501)->second);
    result.type = type;
    result.headers = extractHeaders(request.substr(0, header_end));
    std::vector<std::string> lines = Utils::SplitByEach(request.substr(0, request.find_first_of(CRLF)), " \t");
    result.uri = lines[1];
    is_req_well_formed(result, servers, socket_fd);
    std::string locate_uri = getLocationByUri(result.handler.locations, result.uri);
    if (locate_uri.empty() && result.type != GET)
        throw HttpException(http_codes.find(405)->first, http_codes.find(405)->second);
    else
        validateLocation(locate_uri, result);
    return result;
}


