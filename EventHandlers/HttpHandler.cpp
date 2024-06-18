#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler() : EventHandler(-1) {}

HttpHandler::HttpHandler(int client_fd, const std::vector<ServerConf> &ServerConfs) : EventHandler(client_fd, ServerConfs), start(clock())
{
	read_state = READ;
	headers_done = 0;
	status_code = 200;
	throwing = 0;
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
	this->rest = m_data.request.substr(index + 4);
	m_data.request = m_data.request.substr(0, index);
	this->headers_done = 1;
}


void HttpHandler::Read()
{
	throwing = 0;
	char buffer[READ_SIZE];
	int readed = read(this->socket_fd, buffer, READ_SIZE - 1);
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
	// if (this->rest.length())
	// {
	// 	this->m_data.temp_fd = open("tempfile", O_RDWR | O_CREAT | O_TRUNC, 0600);
	// 	if (this->m_data.temp_fd < 0)
	// 	{
	// 		std::cerr << "failed to open temp file" << std::endl;
	// 		this->status_code = 500;
	// 		this->read_state = WRITE;
	// 		return ;
	// 	}
	// 	this->rest.clear();
	// }
	if (throwing){
		std::cout << "dropping" << std::endl;
		return;
	}

	try
	{
		// std::cout << "parsing" << std::endl;
		Parse(m_data.request, this->ServerConfs, this->socket_fd, m_data);
		this->read_state = WRITE;
		throwing = 1;
	}
	catch (const HttpException& e)
	{
		this->status_code = e.getCode();
		this->read_state = WRITE;
		// std::cerr << e.what() << std::endl;
	}
}

void HttpHandler::Write()
{
	std::cout << "trying to write" << std::endl;
	std::stringstream ss;
	std::string response;
	read_state = CLOSE;
	if (this->status_code >= 400 && this->status_code <= 511)
	{
		ss << status_code;
		response += "HTTP/1.1 " + ss.str() + " " + http_codes[status_code].substr(4) + DCRLF;
		response += Utils::getErrorcode(status_code);
		write(this->socket_fd, response.c_str(), response.length());
		response.clear();
		return;
	}
	std::cout << "writing ok" << std::endl;
	ss << 200;
	response += "HTTP/1.1 " + ss.str() + " " + http_codes[status_code] + DCRLF;
	response += Utils::getErrorcode(status_code);
	write(this->socket_fd, response.c_str(), response.length());
	response.clear();
}

int HttpHandler::handleEvent(uint32_t event)
{
	(void) event;
	if (event & EPOLLIN)
		Read();
	else
		Write();
	return (this->read_state);
	// switch (read_state)
	// {
	// 	case READ:
	// 		Read();
	// 		break;
	// 	case WRITE:
	// 		Write();
	// 		break;
	// 	case CLOSE:
	// 		return CLOSE;
	// 	default : break;
	// }
	// return (read_state);
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

