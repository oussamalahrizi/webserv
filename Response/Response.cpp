#include "../includes/main.hpp"


Response::Response() {}

Response::Response(int code, const std::string& message) : status_code(code), status_message(message) {}

Response::Response(const Response& other)
{
	*this = other;
}

Response& Response::operator=(const Response& other)
{
	if (this != &other)
	{
		status_code = other.status_code;
		status_message = other.status_message;
		headers = other.headers;
		body = other.body;
	}
	return (*this);
}

void Response::set_status(int code, const std::string& message)
{
    status_code = code;
    status_message = message;
}

void Response::add_header(const std::string& name, const std::string& value)
{
    headers[name] = value;
	
}

void Response::set_body(const std::string& b)
{
	body = b;
}

std::string Response::to_string() const
{
	std::stringstream ss;
	ss << status_code;
	std::string response = "HTTP/1.1 " + std::string(ss.str()) + " " + status_message + "\r\n";
	for(std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n" + body;
	return response;
}

Response::~Response() {}