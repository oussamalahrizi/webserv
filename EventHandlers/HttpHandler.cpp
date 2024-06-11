#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler() : EventHandler(-1) {}

HttpHandler::HttpHandler(int client_fd, const std::vector<ServerConf> &ServerConfs) : EventHandler(client_fd, ServerConfs), start(clock())
{
	body_fd = -1;
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
}


int HttpHandler::readBody(int flag)
{
	char buffer[1025];
	static int f = 0;
	if (flag)
	{
		// preserve Body by content length;
		size_t to_read = this->content_length >= 1024 ? 1024 : this->content_length;
		size_t readed = recv(this->socket_fd, buffer, to_read, 0);
		this->content_length -= readed;
		if (readed < to_read)
			this->read_state = DONE;
		this->body.clear();
		this->body.append(buffer, readed);
		if (write(body_fd, this->body.c_str(), this->body.length()) < 0)
			throw std::runtime_error("cant write to body fd");
		return (1);
	}
	// throw body
	if (!f)
	{
		std::cout << "THROWING THE BODY, content length :" << content_length << std::endl;
		f = 1;
	}
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
		return this->readBody(1);
	return (1);
}

std::string HttpHandler::getFullRequest() const
{
	return this->request;
}

int HttpHandler::buildTimeout()
{
	send(this->socket_fd, "HTTP/1.1 408 Request Timeout\n\r", 31, 0);
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
	// std::cout << "writing response" << std::endl;
	std::string res = "HTTP/1.1 200 OK";
	std::string body = "<html><body><h1>Hello world!</h1></body></html>";
	res+= CRLF;
	res+= "Content-Type: " + mimetype.find(".html")->second;
	res+= CRLF;
	res+= "Connection: close";
	res+= DCRLF;
	res+= body;
	send(this->socket_fd, res.c_str(), res.length(), 0);
	if (body_fd != -1)
		close(this->body_fd);
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
		data result = Parse(this->request, this->ServerConfs, this->socket_fd);	
	}
	catch (const HttpException& e)
	{
		// make response based on error code;
		std::cout << "error code : " << e.getCode() << std::endl;
		std::cout << e.what()<< std::endl;
		this->read_state = DONE;
	}
}

void HttpHandler::openBodyFile()
{
	std::cout << "open temp file to write body" << std::endl;
}