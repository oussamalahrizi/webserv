#include "main.hpp"

Location::Location()
{
	this->initMap();
	this->root = "";
	this->redirect = "";
	this->first = 0;
}

Location::Location(Server &server)
{
	this->initMap();
	this->root = server.root;
	this->redirect = "";
	this->first = 0;
	this->met = 0;
}

Location::Location(std::string nestedPath)
{
	(void) nestedPath;
}

Location::~Location() {}

void Location::initMap()
{
	Location::Directives.insert(std::make_pair("location", &Location::ValidatePath));
	Location::Directives.insert(std::make_pair("root", &Location::validateRoot));
	Location::Directives.insert(std::make_pair("error_page", &Location::validateErrors));
	Location::Directives.insert(std::make_pair("autoindex", &Location::validateAutoindex));
	Location::Directives.insert(std::make_pair("allow", &Location::validateMethods));
	Location::Directives.insert(std::make_pair("return", &Location::validateRedirect));
}

void Location::ValidateDirective(const std::string& token)
{
	std::vector<std::string> splited = Utils::SplitByEach(token, " \t");
	std::map<std::string, void (Location::*)(const std::vector<std::string>&)>
		::iterator it = this->Directives.find(splited[0]);
	if (it == this->Directives.end())
		throw std::runtime_error("unknown location directive " + splited[0]);
	if (splited[0] == "location")
		(this->*(it->second))(std::vector<std::string>(splited.begin(), splited.end()));
	else
		(this->*(it->second))(std::vector<std::string>(splited.begin() + 1, splited.end()));
}


void Location::ValidatePath(const std::vector<std::string> &rest)
{
	if (rest.size() != 2)
		throw std::runtime_error("invalid location usage");
	this->path = rest[1];
	this->Directives.erase(this->Directives.find("location"));
}

void Location::validateRoot(const std::vector<std::string> &rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("multiple/empty roots");
	if (!this->root.empty())
		throw std::runtime_error("root already defined");
	this->root = rest[0];
}


void Location::validateErrors(const std::vector<std::string> &rest)
{
	if (rest.size() != 2)
		throw std::runtime_error("wrong usage of error page list");
	if (!Utils::CheckNumeric(rest[0], 3))
		throw std::runtime_error("invalid code value for error page");
	this->error_pages[atoi(rest[0].c_str())] = rest[1];
}

void Location::validateAutoindex(const std::vector<std::string> &rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("wrong usage of autoindex");
	if (rest[0] != "on" && rest[0] != "off")
		throw std::runtime_error("autoindex must be on/off");
	if (!first)
	{
		this->autoindex = rest[0] == "on" ? true : false;
		first = 1;
	}
	else
		throw std::runtime_error("autoindex duplicate");
}

void Location::validateMethods(const std::vector<std::string> &rest)
{
	if (!rest.size())
		throw std::runtime_error("no method provided");
	if (!this->met)
		this->met = 1;
	for (size_t i = 0; i < rest.size(); i++)
	{
		if (rest[i] == "GET")
		{
			std::vector<Method>::iterator it = std::find(this->methods.begin(),
				this->methods.end(), GET);
			if (it != this->methods.end())
				throw std::runtime_error("duplicate method GET");
			this->methods.push_back(GET);
		}
		else if (rest[i] == "POST")
		{
			std::vector<Method>::iterator it = std::find(this->methods.begin(),
				this->methods.end(), POST);
			if (it != this->methods.end())
				throw std::runtime_error("duplicate method POST");
			this->methods.push_back(POST);
		}
		else if (rest[i] == "DELETE")
		{
			std::vector<Method>::iterator it = std::find(this->methods.begin(),
				this->methods.end(), DELETE);
			if (it != this->methods.end())
				throw std::runtime_error("duplicate method DELETE");
			this->methods.push_back(DELETE);
		}
		else
		{
			std::cout << "|" + rest[i] + "|" << std::endl;
			throw std::runtime_error("unknown method");
		}
	}
}

void Location::validateRedirect(const std::vector<std::string> &rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("empty/multiple redirect");
	if (!this->redirect.empty())
		throw std::runtime_error("multple returns in location");
	this->redirect = rest[0];	
}

