#include "../includes/main.hpp"

RequestParser::RequestParser() {};


RequestParser::~RequestParser() {}

void getFirstLine(std::string& line)
{
	std::vector<std::string> split = Utils::Split(line, " ");
	std::cout << "Method type : " << split[0] << std::endl;
	std::cout << "Path : " << split[1] << std::endl;
	std::cout << "Http version : " << split[2] << std::endl;
}

std::map<std::string, std::string> RequestParser::Parse( std::string& request)
{
	size_t header_end = request.find(DCRLF);
	std::map<std::string, std::string> headers;
	std::vector<std::string> lines;
	std::string key, value;
	if (header_end != std::string::npos)
	{
		std::string without = request.substr(0, header_end); // header - 0;
		std::vector<std::string> lines = Utils::Split(without, CRLF);
		std::vector<std::string>::iterator it = lines.begin();
		it++;
		while (it != lines.end())
		{
			size_t index = (*it).find(":");
			key = (*it).substr(0, index); // index - 0
			value = (*it).substr(index + 1);
			key = Utils::Trim(key);
			value = Utils::Trim(value);
			headers[key] = value;
			it++;
		}
		return (headers);
	}
	throw std::runtime_error("end of headers not found");
	return (headers);
}


Method RequestParser::GetRequestType(std::string& request)
{
	std::string which = request.substr(0, request.find(" ")); // again - 0
	if (which == "GET")
		return GET;
	else if (which == "POST")
		return POST;
	else if (which == "DELETE")
		return DELETE;
	else
		return OTHER;
}
