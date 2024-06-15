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


int isHex(const std::string& sub)
{
	size_t i = 0;
	while (i < sub.length())
	{
		if (!std::isxdigit(sub[i]))
			return (0);
		i++;
	}
	return (1);
}

int HttpHandler::readBody(int flag)
{
	char buffer[65537];
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
	static int throwing = 0;
	if (!f)
	{
		std::cout << "THROWING THE BODY, content length :" << content_length << std::endl;
		f = 1;
	}
	size_t readed = recv(this->socket_fd, buffer, 65536, 0);
	if (readed <= 0)
	{
		std::cout << "client failed" << std::endl;
		this->read_state = DONE;
		return (0);
	}
	if (throwing)
	{
		this->read_state = DONE;
		return (1);
	}
	this->start = clock();
	this->body.append(buffer, readed);
	static int value = 0;
	size_t pos = 0;
	static int chunked = 0;
	static unsigned long long total = 0;
	while (!body.empty())
	{
		if (chunked == 0)
		{
			pos = body.find(CRLF);
			if (pos == std::string::npos)
			{
				std::cerr << "error finding crlf" << std::endl;
				break;
			}
			std::string sub = body.substr(0, pos);
			std::stringstream ss(sub);
			ss << std::hex << sub;
			ss >> value;
			if (ss.fail())
			{
				std::cerr << "error reading chunk size" << std::endl;
				break;
			}
			std::cout << "chunk size :" << value << std::endl;
			if (value == 0)
			{
				this->body.clear();
				read_state = DONE;
				break;
			}
			chunked = 1;
			body = body.erase(0, pos + 2);
		}
		else
		{
			int rem;
			if (value != 0)
			{
				rem = ((size_t) value) > body.length() ? body.length() : value;
				value -= rem;
				std::string sub = body.substr(0, rem);
				write(this->body_fd, sub.c_str(), sub.length());
				body = body.erase(0, rem);
				total += rem;
				if (total >= result.handler.max_body_size)
				{
					std::cout << "max size exceeded" << std::endl;
					this->status_code = 413;
					this->status_message = http_codes.find(413)->second;
					throwing = 1;
					this->body.clear();
					break;
				}
			}
			if (body.length() < 2)
				break;
			if (value == 0)
			{
				if (body.length() > 2 && body.substr(0, 2) != CRLF)
				{
					std::cerr << "error chunked crlf" << std::endl;
					pos = body.find(CRLF);
					if (pos == std::string::npos)
					{
						chunked = 0;
						this->body.clear();
						break;
					}
					body = body.erase(0, pos + 2);
					chunked = 0;
					continue;
				}
				body = body.erase(0, 2);
				chunked = 0;
			}
		}
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
		// client error
		this->read_state = DONE;
		return (0);
	}
	this->request.append(buffer, readed);
	if (this->request.find(DCRLF) != std::string::npos)
	{
		size_t index = this->request.find(DCRLF);
		this->body = this->request.substr(index + 4);
		this->request = this->request.substr(0, index + 4);
		std::cout << "FOUND HEADERS" << std::endl;
		this->parseHeaders();
		this->body_fd = open("./video.mp4", O_RDWR | O_CREAT | O_TRUNC, 0644);
	}
	return (1);
}

int HttpHandler::Read()
{
	if (read_state == READING)
		return (this->readHeaders());
	else if (read_state == BODY)
		return this->readBody(0);
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
		std::cout << "timeout" << std::endl;
		return buildTimeout();
	}
	if (this->read_state != DONE)
		return (1);
	std::cout << "writing response" << std::endl;
	close(this->body_fd);
	std::stringstream ss;
	ss << this->status_code;
	std::string res = "HTTP/1.1 " + ss.str() + " " + this->status_message;
	std::string body = "<html><body><h1>"+ this->status_message +"</h1></body></html>";
	res+= CRLF;
	res+= "Content-Type: " + mimetype.find(".html")->second;
	res+= CRLF;
	res+= "Connection: close";
	res+= DCRLF;
	res+= body;
	send(this->socket_fd, res.c_str(), res.length(), 0);
	return (0);
}

int HttpHandler::checkTimeout()
{
	if (this->read_state == READING
		&& (clock() - this->start > TIMEOUT_HEADERS * CLOCKS_PER_SEC))
		return (1);
	if (this->read_state == BODY
		&& clock() - this->start > TIMEOUT_HEADERS * CLOCKS_PER_SEC)
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
		result = Parse(this->request, this->ServerConfs, this->socket_fd);
		this->status_code = 200;
		this->status_message = "OK";
		this->read_state = BODY;
	}
	catch (const HttpException& e)
	{
		// make response based on error code;
		std::cout << "error code : " << e.getCode() << std::endl;
		std::cout << e.what()<< std::endl;
		this->read_state = DONE;
		this->status_code = e.getCode();
		this->status_message = e.what();
	}
}

void HttpHandler::openBodyFile()
{
	std::cout << "open temp file to write body" << std::endl;
}

/*
	GET / HTTTP/1.1\r\n
	Host: localhost:3000\r\n
	Transfer-encoding: chunked\r\n
	\r\n

*/

