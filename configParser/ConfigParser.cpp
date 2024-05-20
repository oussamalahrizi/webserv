#include "includes/ConfigParser.hpp"

ConfigParser::ConfigParser() {
	this->token_index = -1;
}

ConfigParser::~ConfigParser()
{
}


void ConfigParser::Init(const std::string &filename)
{
	file.open(filename.c_str());
  	if (!file.is_open() || !file.good())
		throw std::runtime_error("cannot open the config file");
	DoStuff();
}

void ConfigParser::DoStuff()
{
	
	std::stringstream ss;
	std::string content;
	size_t index = 0;
	ss << file.rdbuf();
	content = ss.str();
	Tokenize(content, index);
	for (size_t i = 0; i < tokens.size(); i++)
	{
		tokens[i] = Utils::Trim(tokens[i]);
		if (tokens[i].empty())
			tokens.erase(tokens.begin() + i);
		std::cout << tokens[i] << std::endl;
	}
	while (1);
	CheckServer();
	this->ValidateDirectives();
}

void skip_spaces(std::string &content, size_t &i)
{
	while (content[i] && std::isspace(content[i]))
		i++;
}

int is_delimiter(char c)
{
	return ((c == ';' || c == '#'
		||  c == '{' || c == '{') ? 1 : 0);
}

int hasMoreTokens(size_t index, std::string& input)
{
	return index < input.length();
}


void ConfigParser::Tokenize(std::string& content, size_t& i)
{
	if (!content[i])
		return;
	
	skip_spaces(content, i);

	if (content[i] == ';')
	{
		tokens.push_back(std::string(";"));
		while (content[i] && content[i] != '\n')
			i++;
		if (content[i] == '\n')
			i++;
	}
	else if (content[i] == '#')
	{
		i++;
		while (content[i] && content[i] != '\n')
			i++;
		if (content[i] == '\n')
			i++;
	}
	else if (content[i] == '}' || content[i] == '{')
	{
		tokens.push_back(std::string(content.substr(i, 1)));
		i++;
	}
	else
	{
		size_t pos = i;
		while (content[i] && content[i] != '\n' && !is_delimiter(content[i]))
			i++;
		tokens.push_back(content.substr(pos, i - pos));
	}
	Tokenize(content, i);
}


std::string& ConfigParser::nextToken()
{
	static std::string empty = "";
	this->token_index++;
	if (token_index >= tokens.size())
		return empty;
	return (this->tokens[token_index]);
}

void ConfigParser::ValidateDirectives()
{
	std::string current;
	Server server;
	size_t i;
	for (i = token_index + 1; i < tokens.size() && (tokens[i] != "{"); i++);
	if (i >= tokens.size())
		return;
	this->token_index = i;
	current = this->nextToken();
	while (true)
	{
		if (current.substr(0, 8) == "location")
		{
			// TODO pass the list of tokens and check inside closing braces;
			// TODO : skip until end of brace;
			while(current != "}")
			{
				Location location(current);
				location.validateLocation(current);
			}

		}
		server.validateDirective(current);
		current = nextToken();
		if (current != ";")
			throw std::runtime_error("must end with semicolon");
		current = nextToken();
		if (current == "" || current == "server")
			break;
	}
	server.validateEverything();
	this->servers.push_back(server);
	this->ValidateDirectives();
}


ConfigParser& ConfigParser::operator=(const ConfigParser&other)
{
	(void) other;
	return *this;
}

void ConfigParser::CheckServer()
{
	size_t i = 0;
	int brace_count;
	while (i < tokens.size())
	{
		brace_count = 0;
		if (tokens[i] != "server")
			throw std::runtime_error("unknown global directive " + tokens[i]);
		i++;
		if (tokens[i] != "{")
			throw std::runtime_error("missing opening brace for server block" + tokens[i]);
		i++, brace_count++;
		size_t j = i;
		while (j < tokens.size())
		{
			if (tokens[j] == "{")
				brace_count++;
			else if (tokens[j] == "}")
				brace_count--;
			if (brace_count == 0)
				break;
			j++;
		}
		if (brace_count > 0)
			throw std::runtime_error("missing closing brace for server block");
		else if (brace_count < 0)
			throw std::runtime_error("missing opening brace for server block");
		i = j + 1;
	}
}