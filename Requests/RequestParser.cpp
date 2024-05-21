#include "includes/RequestParser.hpp"

RequestParser::RequestParser() {};


RequestParser::~RequestParser() {}

std::map<std::string, std::string> RequestParser::GetHeaders() const
{
	return (this->headers);
}

void getFirstLine(std::string& line)
{
	std::vector<std::string> split = Utils::Split(line, " ");
	std::cout << "Method type : " << split[0] << std::endl;
	std::cout << "Path : " << split[1] << std::endl;
	std::cout << "Http version : " << split[2] << std::endl;
}

int RequestParser::Parse( std::string& request)
{
	size_t header_end = request.find(DCRLF);
	std::vector<std::string> lines;
	std::string key, value;
	if (header_end != std::string::npos)
	{
		std::string without = request.substr(0, header_end); // header - 0;		
		std::vector<std::string> lines = Utils::Split(without, CRLF);
		std::vector<std::string>::iterator it = lines.begin();
		getFirstLine(*it);
		it++;
		while (it != lines.end())
		{
			size_t index = (*it).find(":");
			key = (*it).substr(0, index); // index - 0
			value = (*it).substr(index + 1);
			this->headers[key] = value;
			it++;
		}
		std::map<std::string, std::string>::iterator it1 = this->headers.begin();
		std::cout << "Key | Value" << std::endl;
		while (it1 != this->headers.end())
		{
			std::cout << it1->first << " : ";
			std::cout << it1->second << std::endl;
			it1++;
		}
		return (1);
	}
	return (0);
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
		return GET;
}
