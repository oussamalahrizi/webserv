#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler() : EventHandler(-1) {}

HttpHandler::HttpHandler(int client_fd, const std::vector<ServerConf> &ServerConfs) : EventHandler(client_fd, ServerConfs), start(clock())
{
	read_state = READ;
	headers_done = 0;
	status_code = 200;
	m_data.trans = NONE;
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
	this->rest = this->m_data.request.substr(index + 4);
	m_data.request = m_data.request.substr(0, index);
	this->headers_done = 1;
}

void HttpHandler::handleBody()
{
	this->start = clock();
	try
	{
		if (m_data.trans == LENGTH && cl->transfer(this->rest))
		{
			std::cout << "reading cl done" << std::endl;
			read_state = WRITE;
			delete cl;
		}
		else if (m_data.trans == CHUNKED && chunked->transfer(this->rest))
		{
			read_state = WRITE;
			delete chunked;
		}
	}
	catch (const HttpException& e)
	{
		this->read_state = WRITE;
		this->status_code = e.getCode();
	}
}

void HttpHandler::Read()
{
	char buffer[READ_SIZE];
	size_t readed = recv(this->socket_fd, buffer, READ_SIZE - 1, 0);
	if (readed <= 0)
	{
		std::cerr << "client failed" << std::endl;
		this->read_state = CLOSE;
		return;
	}
	if (read_state == WRITE)
	{
		std::cout << "discarding\n";
		return;
	}
	if (read_state == BODY)
	{
		this->rest.append(buffer, readed);
		this->handleBody();
		this->rest.clear();
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
			Parse(m_data.request, this->ServerConfs, socket_fd, m_data);
			if (m_data.trans == CHUNKED || m_data.trans == LENGTH)
			{
				std::cout << "here" << std::endl;
				this->openTempFile();
				read_state = BODY;
				if (m_data.trans == LENGTH)
					cl = new LengthBody(m_data);
				else if (m_data.trans == CHUNKED)
					chunked = new ChunkedBody(m_data);
			}
			else
				read_state = WRITE;
		}
		catch (const HttpException& e)
		{
			this->status_code = e.getCode();
			read_state = WRITE;
		}
	}
}

void HttpHandler::Write()
{
	if (read_state == READ && clock() - this->start > CLOCKS_PER_SEC * TIMEOUT_HEADERS)
	{
		read_state = WRITE;
		status_code = 408;
	}
	if (read_state == BODY && clock() - this->start > CLOCKS_PER_SEC * TIMEOUT_BODY)
	{
		read_state = WRITE;
		status_code = 408;
	}
	if (read_state != WRITE)
		return;
	std::stringstream ss;
	std::string response;
	read_state = CLOSE;
	if (this->status_code >= 400 && this->status_code <= 511)
	{
		std::cout << "writing bad" << std::endl;
		ss << status_code;
		response += "HTTP/1.1 " + ss.str() + " " + http_codes[status_code].substr(4) + CRLF;
		response += std::string("Location: localhost:3000/") + DCRLF;
		response += Utils::getErrorcode(status_code);
		send(this->socket_fd, response.c_str(), response.length(), 0);
		response.clear();
		return;
	}
	std::cout << "writing ok" << std::endl;
	ss << 200;
	response += "HTTP/1.1 " + ss.str() + " " + http_codes[status_code] + DCRLF;
	response += Utils::getErrorcode(status_code);
	if (send(this->socket_fd, response.c_str(), response.length(), 0) < 0)
		throw std::runtime_error("send failed");
	response.clear();
}

int HttpHandler::handleEvent(uint32_t event)
{
	if (event & EPOLLIN)
		Read();
	else if (event & EPOLLOUT)
		Write();
	return (read_state);
}

void HttpHandler::openTempFile()
{
	
	this->m_data.tempfile_name = "temps/" +  UUID::generate();
	if (m_data.headers.find("Content-Type") != m_data.headers.end())
	{
		std::map<std::string, std::string>::iterator it = mimetype.find(m_data.headers.find("Content-Type")->second);
		if (it != mimetype.end())
			this->m_data.tempfile_name.append(it->second);
		else
			this->m_data.tempfile_name.append(".txt");
	}
	this->m_data.temp_fd = open(this->m_data.tempfile_name.c_str(),
		O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (m_data.temp_fd < 0)
		throw HttpException(500);
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

