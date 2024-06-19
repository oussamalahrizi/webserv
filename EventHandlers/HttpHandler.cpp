#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler() : EventHandler(-1) {}

HttpHandler::HttpHandler(int client_fd, const std::vector<ServerConf> &ServerConfs) : EventHandler(client_fd, ServerConfs), start(clock())
{
	read_state = READ;
	headers_done = 0;
	status_code = 200;
	parse_done = 0;
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


EventHandler *HttpHandler::Accept()
{
	return (NULL);
}

void HttpHandler::readHeaders()
{
	size_t index = m_data.request.find(DCRLF);
	if (index == std::string::npos) return;
	std::cout << "headers end" << std::endl;
	this->read_state = WRITE;
	this->rest = m_data.request.substr(index + 4);
	m_data.request = m_data.request.substr(0, index);
	this->headers_done = 1;
}


void printstate(int r)
{
	switch(r)
	{
		case READ:
			std::cout << "READ" << std::endl;
			break;
		case WRITE:
			std::cout << "WRITE" << std::endl;
			break;
		case CLOSE:
			std::cout << "CLOSE" << std::endl;
			break;
		default : break;
	}
}

void HttpHandler::Read()
{
	char buffer[READ_SIZE];
	int readed = recv(this->socket_fd, buffer, READ_SIZE - 1, 0);
	if (readed <= 0)
	{
		std::cerr << "client failed" << std::endl;
		this->read_state = CLOSE;
		return;
	}
	if (!headers_done)
	{
		this->m_data.request.append(buffer, readed);
		readHeaders();
	}
	if (headers_done)
	{
		try
		{
			Parse(m_data.request, this->ServerConfs, this->socket_fd, m_data);
			content_length = 0;
		}
		catch (const HttpException& e)
		{
			status_code = e.getCode();
			read_state = WRITE;
			content_length = 0;
		}
	}
}

void HttpHandler::Write()
{
	std::cout << "writing" << std::endl;
	std::stringstream ss;
	std::string response;
	read_state = CLOSE;
	if (this->status_code >= 400 && this->status_code <= 511)
	{
		ss << status_code;
		response += "HTTP/1.1 " + ss.str() + " " + http_codes[status_code].substr(4) + DCRLF;
		response += Utils::getErrorcode(status_code);
		send(this->socket_fd, response.c_str(), response.length(), 0);
		response.clear();
		return;
	}
	std::cout << "writing ok" << std::endl;
	ss << 200;
	response += "HTTP/1.1 " + ss.str() + " " + http_codes[status_code] + DCRLF;
	response += Utils::getErrorcode(status_code);
	send(this->socket_fd, response.c_str(), response.length(), 0);
	response.clear();
}

int HttpHandler::handleEvent(uint32_t event)
{
	printstate(read_state);
	switch (read_state)
	{
		case READ:
			if (event & EPOLLIN)
				Read();
			break;
		case WRITE:
			if (event & EPOLLOUT)
				Write();
			break;
		case CLOSE:
			return (CLOSE);
		default:
			break;
	}
	return (this->read_state);
}

/*
	GET /file HTTP/1.1\r\n
	Host: localhost:3000\r\n
	Transfer-encoding: chunked\r\n
	\r\n
	1\r\n
	data\r\n
	4\r\n
	d\r\n
	4\r\n
	d\r\n
	4\r\n
	d\r\n
*/

