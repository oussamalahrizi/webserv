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
}

int HttpHandler::Read()
{
	char buffer[1025];

	size_t readed = read(this->socket_fd, buffer, 1024);
	if (readed <= 0)
	{
		this->read_state = DONE;
		// failed or connection closed by client
		return (0);
	}
	if (read_state != READING && read_state != BODY)
		return (1);
	// append up to the bytes readed without \0 to avoid messing up binary data
	this->request.append(buffer, readed);
	if (this->request.find(DCRLF, request.size() - readed) != std::string::npos)
	{
		parseHeaders();
		return (1);
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
	httpResponse->add_header("Content-Type", "text/html");
	httpResponse->add_header("Connection", "close");
	httpResponse->set_body("<html><body><h1>" + httpResponse->status_message + "</h1></body></html>");
	std::string buf = httpResponse->to_string();
	write(this->socket_fd, buf.c_str(), buf.length());
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
			this->read_state = DONE;
		else
		{
			this->content_length = parse.getContentLength();
			read_state = BODY;
		}
		// check Request body larger than client max body size in conf file
		// if (parse.GetRequestType() == POST)
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		std::string err = e.what();
		this->httpResponse = new Response(std::atoi(err.substr(3).c_str()), e.what() + 4);
	}
}