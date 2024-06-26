#include "../includes/main.hpp"

ConfigParser::ConfigParser()
{
	this->token_index = -1;
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::Init(const std::string &filename, std::vector<ServerConf> &ServerConfs)
{
	file.open(filename.c_str());
	if (!file.is_open() || !file.good())
		throw std::runtime_error("cannot open the config file");
	DoStuff(ServerConfs);
}

void ConfigParser::DoStuff(std::vector<ServerConf> &ServerConfs)
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
	}
	CheckServerConf();
	this->ValidateDirectives(ServerConfs);
}

void skip_spaces(std::string &content, size_t &i)
{
	while (content[i] && std::isspace(content[i]))
		i++;
}

int is_delimiter(char c)
{
	return ((c == ';' || c == '#' || c == '{' || c == '}') ? 1 : 0);
}

int hasMoreTokens(size_t index, std::string &input)
{
	return index < input.length();
}

void ConfigParser::Tokenize(std::string &content, size_t &i)
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

std::string &ConfigParser::nextToken()
{
	static std::string empty = "";
	this->token_index++;
	if (token_index >= tokens.size())
		return empty;
	return (this->tokens[token_index]);
}

void ConfigParser::LocationLexer(std::string &current, ServerConf *ServerConf, Location *parent)
{
	std::vector<std::string> splited = Utils::SplitByEach(current, " \t");
	if (splited[0] != "location")
		throw std::runtime_error("unknown server directive");
	Location location(*ServerConf);
	location.ValidateDirective(current);
	current = this->nextToken();
	if (current != "{")
		throw std::runtime_error("missing opening brace for location");
	current = this->nextToken();
	while (current != "}")
	{
		if (current.substr(0, 8) == "location")
		{
			// recursive call for nested location until
			// the end of the nested brace then continue
			LocationLexer(current, ServerConf, &location);
			current = nextToken();
			continue;
		}
		location.ValidateDirective(current);
		current = this->nextToken();
		if (current != ";")
			throw std::runtime_error("must end with semicolon");
		current = this->nextToken();
	}
	if (parent)
		location.ValidateEverything(parent);
	else
		location.ValidateEverything(NULL);
	// pushing the location to the ServerConf according to its path
	// need to check for duplicate path locations
	if (ServerConf->locations.find(location.path) != ServerConf->locations.end())
		throw std::runtime_error("duplicate locations");
	if (!parent)
		ServerConf->locations[location.path] = location;
}

void ConfigParser::ValidateDirectives(std::vector<ServerConf> &ServerConfs)
{
	std::string current;
	ServerConf ServerConf;
	size_t i;
	for (i = token_index + 1; i < tokens.size() && (tokens[i] != "{"); i++)
		;
	if (i >= tokens.size())
		return;
	this->token_index = i;
	current = this->nextToken();
	while (current != "}")
	{
		if (current.substr(0, 8) == "location")
		{
			// TODO pass the list of tokens and check inside closing braces;
			// TODO : skip until end of brace;
			LocationLexer(current, &ServerConf, NULL);
			current = nextToken();
			continue;
		}
		ServerConf.validateDirective(current);
		current = nextToken();
		if (current != ";")
			throw std::runtime_error("must end with semicolon");
		current = nextToken();
		if (current == "" || current == "ServerConf")
		{
			std::cout << "breaking : + '" << current << "'" << std::endl;
			break;
		}
	}
	ServerConf.validateEverything();
	ServerConfs.push_back(ServerConf);
	this->ValidateDirectives(ServerConfs);
}

ConfigParser &ConfigParser::operator=(const ConfigParser &other)
{
	(void)other;
	return *this;
}

void ConfigParser::CheckServerConf()
{
	size_t i = 0;
	int brace_count;

	while (i < tokens.size())
	{
		brace_count = 0;
		if (tokens[i] != "server")
			throw std::runtime_error("unknown global directive " + tokens[i]);
		if (i + 1 < tokens.size() && tokens[++i] != "{")
			throw std::runtime_error("missing opening brace for server block " + tokens[i]);
		i++, brace_count++;
		size_t j = i;
		if (i == tokens.size())
			throw std::runtime_error("missing closing brace for server block");
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