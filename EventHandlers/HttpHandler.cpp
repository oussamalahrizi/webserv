#include "../includes/main.hpp"

HttpHandler::HttpHandler() : EventHandler(-1) {}

HttpHandler::HttpHandler(int client_fd, const std::vector<ServerConf> &ServerConfs) : EventHandler(client_fd, ServerConfs),
																					  httpResponse(NULL), start(clock())
{
	read_state = READING;
}

HttpHandler::HttpHandler(const HttpHandler &other) : EventHandler(other)
{
	*this = other;
}

HttpHandler &HttpHandler::operator=(const HttpHandler &other)
{
	if (this != &other)
		EventHandler::operator=(other);
	return *this;
}

HttpHandler::~HttpHandler()
{
	if (httpResponse)
		delete this->httpResponse;
	this->file.close();
}


/*
	GET  / HTTP/1.1
	\r\n
*/

int HttpHandler::readBody(int flag)
{
	char buffer[1025];
	if (flag)
	{
		std::cout << "content remaining : " << this->content_length << std::endl;
		// preserve Body by content length;
		size_t to_read = this->content_length >= 1024 ? 1024 : this->content_length;
		size_t readed = recv(this->socket_fd, buffer, to_read, 0);
		this->content_length -= readed;
		std::cout << "readed :" << readed << std::endl;
		if (readed < to_read)
			this->read_state = DONE;
		this->body.clear();
		this->body.append(buffer, readed);
		file << this->body;
		return (1);
	}
	// throw body
	std::cout << "THROWING THE BODY" << std::endl;
	size_t readed = recv(this->socket_fd, buffer, 1024, 0);
	if (readed <= 0)
	{
		this->read_state = DONE;
		return (0);
	}
	return (1);
}

int HttpHandler::readHeaders()
{
	char buffer[1025];
	std::cout << "READING HEADERS" << std::endl;
	size_t readed = recv(this->socket_fd, buffer, 1024, 0);
	if (readed <= 0)
	{
		this->read_state = DONE;
		return (0);
	}
	this->request.append(buffer, readed);
	if (this->request.find(DCRLF) != std::string::npos)
	{
		size_t index = this->request.find(DCRLF);
		this->request = this->request.substr(0, index + 4);
		std::cout << request << std::endl;
		std::cout << "FOUND HEADERS" << std::endl;
		this->parseHeaders();
	}
	return (1);
}
int HttpHandler::Read()
{
	if (read_state == DONE)
		return (this->readBody(0));
	else if (read_state == READING)
		return (this->readHeaders());
	else if (read_state == BODY)
	{
		if (content_length <= 0)
			return this->readBody(0);
		return this->readBody(1);
	}
	return (1);
}

std::string HttpHandler::getFullRequest() const
{
	return this->request;
}

void HttpHandler::BuildResponse()
{
	httpResponse = new Response(this->status_code, this->message);
	httpResponse->add_header("Content-Type", "text/html");
	httpResponse->add_header("Connection", "close");
	httpResponse->add_header("Cache-Control", "no-store");
	httpResponse->set_body("<html><body><h1>" + message + "</h1></body></html>");
}

int HttpHandler::buildTimeout()
{
	httpResponse = new Response(408, "request timeout");
	httpResponse->set_body("request timeout");
	std::string buf = httpResponse->to_string();
	write(this->socket_fd, buf.c_str(), buf.length());
	return (0);
}

int HttpHandler::Write()
{
	if (checkTimeout())
	{
		std::cout << "timeout";
		return buildTimeout();
	}
	if (this->read_state != DONE)
		return (1);
	if (!httpResponse)
		return (1);
	// std::cout << "writing response" << std::endl;
	httpResponse->add_header("Content-Type", "text/html");
	httpResponse->add_header("Connection", "close");
	httpResponse->add_header("Cache-Control", "no-store");
	httpResponse->set_body("<html><body>" + httpResponse->status_message + "</body></html>");
	std::string buf = httpResponse->to_string();
	send(this->socket_fd, buf.c_str(), buf.length(), 0);
	return (0);
}

int HttpHandler::checkTimeout()
{
	if (this->read_state == READING
		&& (clock() - this->start > TIMEOUT_HEADERS * CLOCKS_PER_SEC))
		return (1);
	return (0);
}

EventHandler *HttpHandler::Accept()
{
	return (NULL);
}

clock_t HttpHandler::getStart() const
{
	return this->start;
}

void HttpHandler::parseHeaders()
{
	try
	{
		ServerConf handler;
		RequestParser parse(this->request);
		Method type = parse.GetRequestType();
		// if (type == POST)
			//go read body
		this->httpResponse = parse.Parse(this->ServerConfs, handler);
		if (type != POST)
		{
			std::cout << "STATE IS DONE" << std::endl;
			this->body.clear();
			this->read_state = DONE;
		}
		else
		{
			this->content_length = parse.getContentLength();
			std::cout << "content length : " << content_length << std::endl;
			read_state = BODY;
		}
		// check Request body larger than client max body size in conf file
		// if (parse.GetRequestType() == POST)
	}
	catch (const std::exception &e)
	{
		this->read_state = DONE;
		std::cerr << e.what() << std::endl;
		std::string err = e.what();
		this->httpResponse = new Response(std::atoi(err.substr(3).c_str()), e.what() + 4);
	}
}