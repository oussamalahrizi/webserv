#include "includes/ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const std::string& filename)
{
	this->file.open(filename.c_str());
	if (!file.is_open() || !file.good())
		throw std::runtime_error("cannot open the config file");
	DoStuff();
}

size_t findMatchingBrace(std::string& string, size_t start)
{
	int brace_count = 0;

	for (size_t i = start; i < string.size(); i++)
	{
        if (string[i] == '{')
            brace_count++;
        else if (string[i] == '}')
            brace_count--;
        if (brace_count == 0)
            return i;
    }
	
	// brace count not 0 meaning something missing
	return std::string::npos;
}

void skip_spaces(std::string &string, size_t &index)
{
	while (string[index] && std::isspace(string[index]))
		index++;
	if (!string[index])
		throw std::runtime_error("Error : eof");
}

void ConfigParser::DoStuff()
{
	std::string content;
	int server_count = 0;
	content = std::string(std::istreambuf_iterator<char>(this->file), std::istreambuf_iterator<char>());
	size_t i = 0;
	// TODO : find server block and send the content between braces to a function to parse;
	while (i < content.length())
	{
		try
		{
			skip_spaces(content, i);
		}
		catch (std::exception& e)
		{
			break;
		}
		if (content.substr(i, 6) == "server")
		{
			server_count++;
			i += 6;
			skip_spaces(content, i);
			if (content[i] != '{')
				throw std::runtime_error("missing starting brace");
			std::cout << "found starting brace" << std::endl;
			size_t t = findMatchingBrace(content, i);
			if (t == std::string::npos)
				throw std::runtime_error("missing matching brace for server directive");
			std::cout << "found ending brace" << std::endl;
			std::cout << "in between : " << std::endl;
			i++;
			this->fill(content.substr(i, t - i));
			i = t + 1;
		}
		else
		{
			throw std::runtime_error("unknown general directive");
		}
	}
	std::cout << "server_count : " << server_count << std::endl;
}

void ConfigParser::CheckKeys(std::string& string,const std::string &check)
{
	
}

void ConfigParser::setKeyword(std::string key, std::string value)
{
	if (this->conf.find(key) == this->conf.end())
		throw std::runtime_error("duplicate of " + key);
	this->conf[key] = value;
}

void ConfigParser::fill(std::string content)
{
	std::vector<std::string> vec = Utils::Split(content, "\n");
	std::string line;
	for (size_t i = 0; i < vec.size(); i++)
	{
		line = vec[i];
		if (Utils::Split(line, " ").back() != ";")
			throw std::runtime_error("line must end with ;");
		std::vector<std::string> temp = Utils::Split(line, " ");
		

	}
	while (1);
}