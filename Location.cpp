#include "includes/main.hpp"

Location::Location() {}

Location::Location(Server &server)
{
	this->initMap();
	this->redirect = "";
	this->met = 0;
	this->first = 0;
	this->server = &server;
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
	std::string path = rest[1];
	if (rest.size() != 2)
		throw std::runtime_error("invalid location usage");
	if (path[0] != '/')
		throw std::runtime_error("location path must start with /");
	if (path[path.length() - 1] == '/')
		path.erase(path.end() - 1);
	this->path = path;
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
	if (this->error_pages.find(atoi(rest[0].c_str())) != this->error_pages.end())
		throw std::runtime_error("duplicate error page for code : " + rest[0]);
	this->error_pages[atoi(rest[0].c_str())] = rest[1];
}


void Location::validateAutoindex(const std::vector<std::string> &rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("wrong usage of autoindex");
	if (rest[0] != "on" && rest[0] != "off")
		throw std::runtime_error("autoindex must be on/off");
	if (!this->first)
	{
		this->first = 1;
		this->autoindex = rest[0] == "on" ? true : false;
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
			this->methods.push_back(POST);
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

void Location::ValidateEverything(Location* parent)
{
	if (parent)
	{
		if (this->path.compare(0, parent->path.size(), parent->path))
			throw std::runtime_error("doesnt match the parent path");
		// combine methods or rather add methods of the parent to the child if not there;
		for (size_t i = 0; i < parent->methods.size(); i++)
		{
			std::vector<Method>::iterator it = std::find(methods.begin(), methods.end(), parent->methods[i]);
			if (it == this->methods.end())
				this->methods.push_back(parent->methods[i]);
		}
		// combine error_pages;
		std::map<int, std::string>::const_iterator it = parent->error_pages.begin();
		while (it != parent->error_pages.end())
		{
			if (this->error_pages.find(it->first) != this->error_pages.end())
				this->error_pages[it->first] = it->second;
			it++;
		}
		if (this->root == "")
			this->root = parent->root;
		parent->nestedLocations.push_back(*this);
	}
	else
	{
		if (!this->met)
			this->methods.push_back(GET);
		if (!this->first)
			this->autoindex = false;
		if (!this->error_pages.size())
			this->error_pages = this->server->error_pages;
		if (this->root == "")
			this->root = this->server->root;
	}	
}