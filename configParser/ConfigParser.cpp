#include "includes/ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser()
{
	size_t i = 0;
	while (i < this->conf.locations.size())
	{
		if (this->conf.locations[i].redirect)
			delete this->conf.locations[i].redirect;
		if (this->conf.locations[i].root)
			delete this->conf.locations[i].root;
		i++;
	}
}

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
    std::size_t pos = 0;
    while ((pos = input.find(c, pos)) != std::string::npos)
	{
        std::size_t end = input.find('\n', pos);
        if (end != std::string::npos)
		{
			if (c == ';')
			{
				input.erase(pos + 1, end - pos - 1);
				pos = end + 1;
			}
			else
			{
				input.erase(pos, end - pos);
				pos = end;
			}
		}
        else
		{
            input.erase(pos, input.length() - pos);
			break;
		}
    }
}

void ConfigParser::SplitBySemicolon(std::string content)
{
	std::vector<std::string> lines;

	// just split each line and pass to next function
	content = Utils::Trim(content);
	lines = Utils::SplitByEach(content, "\n");
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines.at(i).empty())
			lines.erase(lines.begin() + i);
		lines[i] = Utils::Trim(lines[i]);
	}

	this->HandleKeywords(lines);
	std::cout << "PRINTING INFORMATIONS : " << std::endl;
	std::cout << "host : " << this->conf.host << std::endl;
	size_t i = 0;
	if (this->conf.server_names.size())
	{
		std::cout << "server names : " << std::endl;
		while (i < this->conf.server_names.size())
		{
			std::cout << i + 1 << " " << this->conf.server_names[i] << std::endl;
			i++;
		}
	}
	i = 0;
	std::cout << "ports : " << std::endl;
	while (i < this->conf.ports.size())
	{
		std::cout << this->conf.ports[i] << std::endl;
		i++;
	}
	i = 0;
	std::cout << "index : " << std::endl;
	while (i < this->conf.index.size())
	{
		std::cout << this->conf.index[i] << std::endl;
		i++;
	}
	i = 0;
	std::cout << "error pages : " << std::endl;
	std::map<int, std::string>::iterator it = this->conf.error_pages.begin();
	while (it != this->conf.error_pages.end())
	{
		std::cout << "code : " << it->first;
		std::cout << "  path : " << it->second << std::endl;
		it++;
	}
	std::cout << "root : " << this->conf.root << std::endl;
	std::cout << "locations : " << std::endl;
	i = 0;
	while (i < this->conf.locations.size())
	{
		location info = this->conf.locations[i];
		std::cout << "path : " << info.path << std::endl;
		if (info.root)
			std::cout << "root : " << *(info.root) << std::endl;
		if (info.autoindex) 
			std::cout << "autoindex : on"  << std::endl;
		else
			std::cout << "autoindex : off"  << std::endl;
		if (info.methods.size())
		{
			std::cout << "Methods allowed :" << std::endl;
			size_t j = 0;
			while (j < info.methods.size())
			{
				std::cout << info.methods[j] << std::endl;
				j++;
			}
		}
		if (info.error_pages.size())
		{
			std::cout << "error pages :" << std::endl;
			std::map<int, std::string>::iterator f = info.error_pages.begin();
			while (f != info.error_pages.end())
			{
				std::cout << "code : " << f->first;
				std::cout << "	value : " << f->second << std::endl;
				f++;
			}
		}
		if (info.redirect)
			std::cout << "redirect : " << *(info.redirect) << std::endl;
		i++;
	}
	std::cout << "-------------" << std::endl;
}



int checkNumeric(std::string& input, int& value)
{
	if (input.length() != 3)
		return (0);
	size_t i = 0;
	while (i < input.length() && std::isdigit(input[i]))
		i++;
	if (input[i])
		return (0);
	value = std::atoi(input.c_str());
	return (1);
}

location fillLocation(std::vector<std::string> &vec)
{
	location locInfo;
	locInfo.root = NULL;
	locInfo.redirect = NULL;
	std::vector<std::string> splited;
	
	// parse the given location information
	for (size_t i = 0; i < vec.size(); i++)
	{
		splited = Utils::SplitByEach(vec[i], " \t");
		if (splited[0] == "location")
		{
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("Location : no path or multiple paths " + vec[i]);
			locInfo.path = splited[1];
		}
		else if (splited[0] == "allow")
		{
			if (splited.size() > 3 ||splited.size() == 1)
				throw std::runtime_error("Location : methods empty or more than 3");
			locInfo.methods.push_back(splited[1]);
		}
		else if (splited[0] == "root")
		{
			if (locInfo.root)
				throw std::runtime_error("Location : duplicate root for location");
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("Location : root empty or more than 1 root");
			locInfo.root = new std::string(splited[1]);
		}
		else if (splited[0] == "return")
		{
			if (locInfo.redirect)
				throw std::runtime_error("Location : duplicate redirect");
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("Location : redirect empty or more that 1 redirect");
			locInfo.redirect = new std::string(splited[1]);
		}
		else if (splited[0] == "autoindex")
		{
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("Location : auto index empty or multiple values");
			if (splited[1] != "on" && splited[1] != "off")
				throw std::runtime_error("Location : invalid value for autoindex");
			locInfo.autoindex = splited[1] == "on" ? true : false;
		}
		else if (splited[0] == "error_page")
		{
			if (splited.size() != 3)
				throw std::runtime_error("Location : error page wrong usage directive");
			int value;
			if (!checkNumeric(splited[1], value))
				throw std::runtime_error("Location : error page number invalid");
			locInfo.error_pages[atoi(splited[1].c_str())] = splited[2];
		}
		else
			throw std::runtime_error("Location : unknown directive " + splited[0]);
	}
	return locInfo;
}

void checkSemiColon(std::string& input)
{
	if (input.substr(0, 8) != "location" && input[input.length() - 1] != ';')
		throw std::runtime_error("line must end with a semicolon " + input);
	if (input.substr(0, 8) != "location")
		input.erase(input.find(';'));
	else if (input[input.length() - 1] == ';' && input.substr(0, 8) == "location")
		throw std::runtime_error("wash nta 7mar azby");
}

void ConfigParser::HandleKeywords(std::vector<std::string> &lines)
{

	std::vector<std::string> splited;
	// std::vector<std::string> location_info;

	for (size_t i = 0; i < lines.size(); i++)
	{
		checkSemiColon(lines[i]);
		splited = Utils::SplitByEach(lines[i], " \t");
		int in_line = 0;
		if (splited[0] == "location")
		{
			location loc;
			if (*(splited.end() - 1) == "{")
			{
				in_line = 1;
				splited.pop_back();
			}
			size_t start = i;
			for(size_t j = i; j < lines.size() ; j++)
			{
				if (lines[j] == "}")
				{
					i = j;
					break;
				}
				if (in_line && j == i + 1)
					checkSemiColon(lines[j]);
				else if (!in_line && j != i)
					checkSemiColon(lines[j]);
			}
			std::vector<std::string> vec(lines.begin() + start, lines.begin() + i);
			// find index of { and remove it :
			for (size_t j = 0; j < vec.size(); j++)
			{
				if (vec[j] == "{")
				{
					vec.erase(vec.begin() + j);
					break;
				}
				else if (*(vec[j].end() - 1) == '{')
				{
					vec[j].erase(vec[j].end() - 1);
					vec[j] = Utils::Trim(vec[j]);
				}
			}
			this->conf.locations.push_back(fillLocation(vec));
		}
		// check if the tested directive is already there and the size of the splited is exactly 2
		else if (splited[0] == "listen")
		{
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("listen empty or multiple ports in a single listen call directive");
			if (splited[1].length() <= 2)
				throw std::runtime_error("invalid port number, must be at least 3 digit number " + splited[1]);
			this->conf.ports.push_back(splited[1]);
		}
		else if (splited[0] == "root" && splited.size() == 2)
		{
			if (splited.size() == 1 || splited.size() > 2)
				throw std::runtime_error("root empty or multiple values directive");
			if (!this->conf.root.empty())
				throw std::runtime_error("duplicate root " + lines[i]);
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
			this->conf.error_pages[atoi(splited[1].c_str())] = splited[2];
		}
		else
			throw std::runtime_error("Server : unknown directive " + lines[i]);
	}
}


ConfigParser& ConfigParser::operator=(const ConfigParser&other)
{
	if (this != &other)
		this->conf = other.conf;
	return *this;
}