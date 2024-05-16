#include "HttpHandler.hpp"

HttpHandler::HttpHandler() : EventHandler(-1) {}

HttpHandler::HttpHandler(int client_fd) : EventHandler(client_fd), httpResponse(NULL) {}

HttpHandler::HttpHandler(const HttpHandler& other) : EventHandler(other)
{
	*this = other;
}

HttpHandler& HttpHandler::operator=(const HttpHandler& other)
{
	if (this != &other)
		EventHandler::operator=(other);
	return *this;
}

HttpHandler::~HttpHandler()
{
	delete this->httpResponse;
}

int HttpHandler::Read()
{
	char buffer[1025];

	int readed = read(this->socket_fd, buffer, 1024);
	if (readed <= 0)
	{
		// failed or connection closed by client
		return (0);
	}
	this->request.append(buffer);
	if (this->parser.Parse(request))
		httpResponse = new Response();
	return (readed);
}

std::string HttpHandler::getFullRequest() const
{
	return this->request;
}

int HttpHandler::Write()
{
	if (!httpResponse)
		return (1);
	httpResponse->set_status(200, "OK");
	httpResponse->add_header("Content-Type", "text/html");
	httpResponse->add_header("Connection", "close");
	httpResponse->set_body("<html><body><h1>Hello, world!</h1></body></html>");
	std::string buf = httpResponse->to_string();
	write(this->socket_fd, buf.c_str(), buf.length());
	return 0;
}

EventHandler* HttpHandler::Accept()
{
	return (NULL);
}


clock_t HttpHandler::getStart() const
{
	return this->start;
}
