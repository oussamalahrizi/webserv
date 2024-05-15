#include "includes/ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const std::string &filename)
{
	this->file.open(filename.c_str());
  	if (!file.is_open() || !file.good())
		throw std::runtime_error("cannot open the config file");
	DoStuff();
}

size_t findMatchingBrace(std::string &string, size_t start)
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

void skip_space(std::string& input, size_t& start)
{
	while (input[start] && std::isspace(input[start]))
		start++;
}


void ConfigParser::DoStuff()
{
	std::stringstream ss;
	// std::string line;
	std::string content;
	// std::vector<std::string> data;
	// int in_server = 0;

	// making the content of file into the string stream so we can use getline into line;
	ss << this->file.rdbuf();
	content = ss.str();
	// remove comments
	this->remove_comments(content, '#');
	this->remove_comments(content, ';');
	// find the server blocks the send it to a helper function
	size_t i = 0;
	while (i < content.length())
	{
		skip_space(content, i);
		if (!content[i])
			break;
		if (content.substr(i, 6) == "server")
		{
			std::cout << "found server" << std::endl;
			i += 6;
			skip_space(content, i);
			if (content[i] != '{' || !content[i])
				throw std::runtime_error("idk wtf is that bro");
			size_t t = findMatchingBrace(content, i);
			if (t == std::string::npos)
				throw std::runtime_error("missing closing brace for server block");
			i++;
			this->SplitBySemicolon(content.substr(i, t - i));
			i = t + 1;
		}
		else
			throw std::runtime_error("directive other than server "
				+ content.substr(i, content.find("\n", i) - i));
	}
}

void printVector(std::vector<std::string> vec)
{
	for (size_t i = 0; i < vec.size(); i++)
		std::cout << i << " " << vec[i] << "|" << std::endl;
}
void ConfigParser::remove_comments(std::string &input, char c)
{
	// remove comments starting from # until a new line
    std::size_t pos;
    while ((pos = input.find(c)) != std::string::npos)
	{
        std::size_t end = input.find('\n', pos);
        if (end != std::string::npos)
            input.erase(pos, end - pos);
        else
            input.erase(pos, input.length() - pos);
    }
}

void ConfigParser::SplitBySemicolon(std::string content)
{
	std::vector<std::string> lines;

	// just split each line and pass to next function

	lines = Utils::SplitByEach(content, " \t");
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines.at(i).empty())
			lines.erase(lines.begin() + i);
	}
	printVector(lines);
	while (1);
	// this->HandleKeywords(lines);
}



int checkNumeric(std::string& input, int& value)
{
	if (input.length() > 3)
		return (0);
	size_t i = 0;
	while (i < input.length() && std::isdigit(input[i]))
		i++;
	if (input[i])
		return (0);
	value = std::atoi(input.c_str());
	return (1);
}

void ConfigParser::HandleKeywords(std::vector<std::string> &lines)
{

	std::vector<std::string> splited;
	printVector(lines);
	while (1);

	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i] = Utils::Trim(lines[i]);
		splited = Utils::SplitByEach(lines[i], " \t");
		// if (splited[0] == "location")
		// {
		// 	for(size_t j = i; j < lines.size() ; j++)
		// 	{
		// 		if (lines[j] == "}")
		// 		{
		// 			i = j + 1;
		// 			break;
		// 		}
		// 	}
		// 	continue;
		// }
		// check if the tested directive is already there and the size of the splited is exactly 2
		if (splited[0] == "listen")
		{
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("listen empty or multiple ports in a single listen call directive");
			this->conf.ports.push_back(splited[1]);
		}
		else if (splited[0] == "root" && splited.size() == 2)
		{
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("root empty or multiple values directive");
			if (!this->conf.root.empty())
				throw std::runtime_error("duplicate root");
			this->conf.root = splited[1];
		}
		else if (splited[0] == "server_name")
		{
			if (splited.size() == 1)
				throw std::runtime_error("server_name empty directive");
			if (this->conf.server_names.size())
				throw std::runtime_error("duplicates server names directives");
			for (size_t j = 1; j < splited.size(); j++)
				this->conf.server_names.push_back(splited[j]);
		}
		else if (splited[0] == "host")
		{
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("host empty or multiple values directive");
			if (!this->conf.host.empty())
				std::cout << "modifying host " << std::endl;
			this->conf.host = splited[1];
		}
		else if (splited[0] == "index")
		{
			if (splited.size() == 1)
				throw std::runtime_error("index empty directive");
			if (this->conf.index.size())
				throw std::runtime_error("duplicate index directives");
			for (size_t j = 1; j < splited.size(); j++)
				this->conf.index.push_back(splited[j]);
		}
		else if (splited[0] == "error_page")
		{
			if (splited.size() != 3)
				throw std::runtime_error("error page wrong usage directive");
			err_pages error;
			int value;
			if (!checkNumeric(splited[1], value))
				throw std::runtime_error("error page number invalid");
			error.err[atoi(splited[1].c_str())] = splited[2];
			this->conf.error_pages.push_back(error);
		}
	}
	
}