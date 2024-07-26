
#include "includes/ErrorPage.hpp"

ErrorPage::ErrorPage(data& payload, int status_code)
{
	gen = 1;
	headers_done = 0;
	code = status_code;
	filename = payload.handler.error_pages[status_code];
	if (access(filename.c_str(), F_OK) || access(filename.c_str(), R_OK)) return;
	fd = open(filename.c_str(), O_RDONLY);
	if (fd < 0) return;
	if (stat(filename.c_str(), &filestat) < 0) return;
	if (S_ISDIR(filestat.st_mode)) return;
	gen = 0;
}

std::string ErrorPage::generate(int code) const
{
	std::string body = "<html>"
		"<head><title> " + http_codes[code] +  "</title></head>"
		"<body>"
		"<center><h1> " + http_codes[code] + " </h1></center>"
		"<br>"
		"<center><h6>CTRL+ALT+DEFEAT</h6></center>"
		"</body>"
		"</html>";
	return body;
}

int ErrorPage::next_chunk(std::string& chunk)
{
	chunk.clear();
	if (!headers_done)
	{
		chunk = "HTTP/1.1 " + http_codes[code] + " " + CRLF;
		chunk += "Connection: close\r\n";
		size_t cl = (gen == 1) ? generate(code).length() : filestat.st_size;
		std::stringstream ss;
		ss << cl;
		chunk += "Content-Length: " +  ss.str() + CRLF;
		chunk += "Content-Type: text/html\r\n";
		chunk += CRLF;
		headers_done = 1;
		return (0);
	}
	if (gen == 1)
	{
		chunk = generate(code);
		return (1);
	}
	char buffer[READ_SIZE];
	int readed = read(fd, buffer, READ_SIZE);
	if (readed < 0)
	{
		std::cerr << "error reading from error page file" << std::endl;
		return (1);
	}
	chunk = std::string(buffer, readed);
	if (readed == 0)
		return (1);
	return (0);
}


ErrorPage::~ErrorPage() {}