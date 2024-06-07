#include "../includes/main.hpp"

RequestParser::RequestParser() {}


RequestParser::RequestParser(const std::string& input) : request(input)
{
	if (!this->checkProtocol(extractLines()[2]))
		throw std::runtime_error("505 invalid http protocol");
}

RequestParser::~RequestParser() {}


Method RequestParser::GetRequestType() const
{
	Method type;
	std::string sub;
	size_t i = 0;
	while (i < this->request.length() && !isspace(this->request[i]))
		i++;
	sub = this->request.substr(0, i);
	if (sub == "GET")
		type = GET;
	else if (sub == "POST")
		type = POST;
	else if (sub == "DELETE")
		type = DELETE;
	else
		type = OTHER;
	return (type);
}

size_t RequestParser::getContentLength()
{
	int c;
	std::map<std::string, std::string>::const_iterator it = this->headers.find("Content-Length");
	std::string value = it->second;
	value = Utils::Trim(value);
	c = std::atoi(value.c_str());
	std::cout << c << std::endl;
	return ((size_t) c);
}

int RequestParser::checkProtocol(std::string& protocol)
{
	if (protocol == "HTTP/1.1")
		return (1);
	return (0);
}

std::vector<std::string> RequestParser::extractLines()
{
	std::string str = this->request.substr(0, this->request.find(CRLF));
	std::vector<std::string> lines = Utils::SplitByEach(str, " \t");
	return (lines);
}

int checkAllowedCHars(const std::string& string)
{
	std::string all("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%");
	size_t i = 0;
	while (i < string.length())
	{
		if (all.find(string[i]) == std::string::npos)
			return (0);
		i++;
	}
	return (1);	
}

int RequestParser::CheckRequestFormed()
{
	// check transfer encoding and content length in post
	if (type == POST)
	{
		std::map<std::string, std::string>::const_iterator it = headers.find("Transfer-Encoding");
		if (it != headers.end() && it->second != "chunked")
		{
			this->response = new Response(501, "not imeplemented");
			return (0);
		}
		if (it == headers.end())
		{
			it = headers.find("Content-Length");
			if (it == headers.end())
			{
				this->response = new Response(400, "bad request");
				return (0);
			}
		}
	}
	// check allowed characters
	if (!checkAllowedCHars(this->uri))
	{
		this->response = new Response(400, "bad request");
		return (0);
	}
	// check uri length more that 2048
	if (this->uri.length() > 2048)
	{
		this->response = new Response(414, "Request-URI Too Long");
		return (0);
	}
	return (1);
}

ServerConf RequestParser::getServerHandler(std::vector<ServerConf>& confs)
{
	std::string hostname, port;
	size_t index = this->host.find(":");
	hostname = this->host.substr(0, index);
	port = this->host.substr(index + 1);
	for (size_t i = 0; i < confs.size(); i++)
	{
		if (confs[i].port == port)
		{
			std::vector<std::string> server_names = confs[i].Server_names;
			for (size_t j = 0; j < server_names.size(); j++)
			{
				if (server_names[j] == hostname)
					return (confs[i]);
			}
		}
	}

	for (size_t i = 0; i < confs.size(); i++)
	{
		if (confs[i].port == port)
			return(confs[i]);
	}
	throw std::runtime_error("500 internal server error");
	return (confs[0]);
}

Response *RequestParser::Parse(std::vector<ServerConf> confs, ServerConf& handler)
{
	std::cout << this->request << std::endl;
	type = this->GetRequestType();
	if (type == OTHER)
		return (new Response(501, "not imeplemented"));
	size_t head_end = this->request.find(DCRLF);
	if (head_end == std::string::npos)
		throw std::runtime_error("400 headers have no end");
	std::vector<std::string> lines = Utils::Split(this->request.substr(0, head_end), CRLF);
	size_t i = 1;
	size_t index;
	std::string key, value;
	while (i < lines.size())
	{
		index = lines[i].find(": ");
		key = Utils::Trim(lines[i].substr(0, index));
		value = Utils::Trim(lines[i].substr(index + 2));
		this->headers[key] = value;
		i++;
	}
	this->host = headers["Host"];
	this->uri = this->extractLines()[1];
	if (!this->CheckRequestFormed())
		return (this->response);
	handler = this->getServerHandler(confs);
	std::string filename = handler.root + this->uri.substr(1);
	if (this->uri == "/")
		filename = handler.root +"index.html";
	std::cout << filename << std::endl;
	// int fd = open(filename.c_str(), O_RDONLY);
	// if (fd < 0)
	// 	return(new Response(404, "not found"));
	Response *res = new Response(200, "OK");
	// int readed = 1;
	// char buffer[1025];
	// std::string body;
	// while (readed > 0)
	// {
	// 	readed = read(fd, buffer ,1024);
	// 	body.append(buffer, readed);
	// }
	// res->set_body(body);
	return (res);
}