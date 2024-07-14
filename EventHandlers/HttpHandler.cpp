#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler() : EventHandler(-1) {}

HttpHandler::HttpHandler(int client_fd, const std::vector<ServerConf> &ServerConfs) : EventHandler(client_fd, ServerConfs), start(clock())
{
	read_state = READ;
	headers_done = 0;
	status_code = 200;
	m_data.trans = NONE;
	res_ready = 0;
	res_finish = 0;
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

void HttpHandler::deleteTempFile()
{
	if (!unlink(m_data.tempfile_name.c_str()))
		std::cout << "temp file deleted" << std::endl;
	else
		std::cout << "temp file cannot be deleted" << std::endl;
}

void HttpHandler::handleBody()
{
	this->start = clock();
	try
	{
		if (m_data.trans == LENGTH && cl->transfer(this->rest))
		{
			std::cout << "reading cl done" << std::endl;
			setState(WRITE);
			delete cl;
			if (m_data.type != POST)
				this->deleteTempFile();
		}
		else if (m_data.trans == CHUNKED && chunked->transfer(this->rest))
		{
			std::cout << "reading chunked done" << std::endl;
			setState(WRITE);
			delete chunked;
			if (m_data.type != POST)
				this->deleteTempFile();
		}
	}
	catch (const HttpException& e)
	{
		setState(WRITE);
		this->status_code = e.getCode();
	}
}

void HttpHandler::setState(state s)
{
	read_state = s;
	if (s == WRITE)
	{
		// prepare response
		std::cout << "time to prepare response" << std::endl;
		prepareResponse();
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
				m_data.temp_fd = -1;
				std::string upload = "";
				if (m_data.type == POST)
				{
					upload = m_data.loc.upload;
					if (upload[upload.length() - 1] == '/')
						upload.erase(upload.length() - 1);
				}
				this->openTempFile(upload);
				setState(BODY);
				if (m_data.trans == LENGTH)
					cl = new LengthBody(m_data);
				else if (m_data.trans == CHUNKED)
					chunked = new ChunkedBody(m_data);
			}
			else
				setState(WRITE);
		}
		catch (const std::runtime_error& e)
		{
			setState(WRITE);
			status_code = -1;
		}
		catch (const HttpException& e)
		{
			this->status_code = e.getCode();
			setState(WRITE);
		}
	}
}

void generate_hello(std::string& chunk)
{
	std::string body = "<html>"
			"<head><title> Hello </title></head>" 
			"<body>" 
			"<center><h1> Hello There </h1></center>"
			"</body>";
	chunk = "HTTP/1.1 " + http_codes[200] + " " + CRLF;
	chunk +=( "Connection: close\r\n");
	chunk += "Content-Type: text/html\r\n";
	std::stringstream ss;
	ss << body.length();
	chunk += "Content-Length: " + ss.str() + "\r\n";
	chunk += CRLF;
	chunk += body;
}

void HttpHandler::Write()
{
	if ( (read_state == READ && clock() - this->start > CLOCKS_PER_SEC * TIMEOUT_HEADERS) 
		|| (read_state == BODY && clock() - this->start > CLOCKS_PER_SEC * TIMEOUT_BODY) )
	{
		read_state = WRITE;
		status_code = 408;
	}
	if (read_state != WRITE)
		return;
	if (status_code == -1)
	{
		// sys call failed when trying to init server handler
		status_code = 500;
		// handle err page generated with code 500
	}
	int finish = 0;
	std::string chunk;
	std::string body;
	static int headers = 0;
	if (isError())
	{
		body = "<html>"
			"<head><title> " + http_codes[status_code] +  "</title></head>" 
			"<body>" 
			"<center><h1> " + http_codes[status_code] + " </h1></center>";
		if (!headers)
		{
			chunk = "HTTP/1.1 " + http_codes[status_code] + " " + CRLF;
			chunk +=( "Connection: close\r\n");
			chunk += "Content-Type: text/html\r\n";
			std::stringstream ss;
			ss << body.length();
			chunk += "Content-Length: " + ss.str() + "\r\n";
			chunk += CRLF;
			headers = 1;
		}
		else
		{
			// handle error page based on either server config or generated
			// when generated it could be sys call fail or server doesnt have page for the code
			chunk = body;
			// hardcoded for now
			finish = 1;
			headers = 0;
		}
	}
	else
	{
		generate_hello(chunk);
		finish = 1;
	}
	std::cout << "-------------------" << std::endl;
	std::cout << chunk << std::endl;
	std::cout << "-------------------" << std::endl;
	send(socket_fd, chunk.c_str(), chunk.length(), 0);
	if (finish)
		setState(CLOSE);
}

int HttpHandler::handleEvent(uint32_t event)
{
	if (event & EPOLLIN)
		Read();
	else if (event & EPOLLOUT)
		Write();
	return (read_state);
}

void HttpHandler::openTempFile(const std::string& upload)
{
	std::string temp =  UUID::generate();
	this->m_data.tempfile_name = m_data.handler.root + "/" + temp;
	if (!upload.empty())
		m_data.tempfile_name = upload + "/" + temp;
	if (m_data.headers.find("Content-Type") != m_data.headers.end())
	{
		std::map<std::string, std::string>::iterator it = mimetype.find(m_data.headers.find("Content-Type")->second);
		if (it != mimetype.end())
			this->m_data.tempfile_name.append(it->second);
	}
	std::cout << "-------------------------------\n";
	std::cout << "temp file name is : " << m_data.tempfile_name << std::endl;
	std::cout << "-------------------------------\n";
	this->m_data.temp_fd = open(this->m_data.tempfile_name.c_str(),
		O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (m_data.temp_fd < 0)
		throw HttpException(500);
}

int HttpHandler::isError()
{
	if (status_code >= 400 && status_code <= 511)
		return (1);
	return (0);
}

void HttpHandler::prepareResponse()
{
	if (isError())
		return;
	if (!m_data.serv_root && m_data.loc.cgi_path != "")
	{
		std::cout << "handle cgi here for this ressouce : " << m_data.ressource
		<< std::endl;
		std::cout <<  "cgi path :" << m_data.loc.cgi_path << std::endl;
		std::cout <<  "cgi extension :" << m_data.loc.cgi_ext << std::endl;
	}
	else if (m_data.type == GET)
	{
		std::cout << "handle GET for this ressouce : " << m_data.ressource << std::endl;
		// try
		// {
		// 	response = new GetRequest(m_data);
		// }
		// catch (const HttpException&e )
		// {
		// 	status_code = e.getCode();
		// 	std::cout << "generate error page";
		// }
	}
	else if (m_data.type == POST)
	{
		std::cout << "handle POST for this ressouce : " << m_data.ressource << std::endl;
	}
	else if (m_data.type == DELETE)
	{
		std::cout << "handle DELETE for this ressouce : " << m_data.ressource << std::endl;
	}
	else
		std::cout << "unexpected error " << std::endl;
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

