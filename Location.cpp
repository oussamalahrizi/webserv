#include "includes/Location.hpp"

Location::Location() {}

Location::Location(ServerConf &conf)
{
	this->initMap();
	this->redirect = "";
	this->met = 0;
	this->first = 0;
	this->conf = &conf;
	this->cgi_ext = "";
	this->cgi_path = "";
	this->upload = "";
	up = 0;

}

std::map<std::string, void (Location::*)(const std::vector<std::string> &)>
	Location::Directives;

Location::~Location() {}


// Location& Location::operator=(const Location& other)
// {
// 	if (this != &other)
// 	{
// 		path = other.path;
// 		root = other.root;
// 		redirect = other.redirect;
// 		redirect_code = other.redirect_code;
// 		error_pages.clear();
// 		error_pages = other.error_pages;
// 		methods.clear();
// 		methods = other.methods;
// 		autoindex = other.autoindex;
// 		nestedLocations.clear();
// 		nestedLocations = other.nestedLocations;
// 		conf = other.conf;
// 	}
// 	return *this;
// }

void Location::initMap()
{
	Location::Directives.insert(std::make_pair("location", &Location::ValidatePath));
	Location::Directives.insert(std::make_pair("root", &Location::validateRoot));
	Location::Directives.insert(std::make_pair("error_page", &Location::validateErrors));
	Location::Directives.insert(std::make_pair("autoindex", &Location::validateAutoindex));
	Location::Directives.insert(std::make_pair("allow", &Location::validateMethods));
	Location::Directives.insert(std::make_pair("return", &Location::validateRedirect));
	Location::Directives.insert(std::make_pair("cgi", &Location::validateCGI));
	Location::Directives.insert(std::make_pair("cgi_path", &Location::validateCgiPath));
	Location::Directives.insert(std::make_pair("upload", &Location::validateUpload));
}

void Location::ValidateDirective(const std::string &token)
{
	std::vector<std::string> splited = Utils::SplitByEach(token, " \t");
	std::map<std::string, void (Location::*)(const std::vector<std::string> &)>::iterator it = this->Directives.find(splited[0]);
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
	std::string path = rest[1];
	if (path[0] != '/')
		throw std::runtime_error("location path starts with a /");
	// check for consecutive slashes
	for (size_t i = 0; i < path.length() - 1; i++)
	{
		if (path[i] == '/' && path[i + 1] == '/')
			throw std::runtime_error("consecutive slashes path");
	}
	if (path != "/" && path[path.length() - 1] == '/')
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
	if (root[root.length() - 1] == '/')
		root.erase(root.length() - 1);
	if (root[0] == '/')
		root.erase(0, 1);
	root = conf->root + root;
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
	else
		throw std::runtime_error("allow duplicate");
	if (rest[0] == "NONE")
	{
		this->methods.clear();
		return;
	}
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
	if (rest.size() != 1 && rest.size() != 2)
		throw std::runtime_error("redirect syntax error");
	if (rest.size() == 2 && rest[0] != "301" && rest[0] != "302" && rest[0] != "303")
		throw std::runtime_error("redirect only supports 301/302/303 error code");
	if (!this->redirect.empty())
		throw std::runtime_error("multiple returns in location");
	if (rest.size() == 2)
	{
		rest[0] == "301" ? redirect_code = 301 : rest[0] == "302" ? redirect_code = 302
			: redirect_code = 303;
		this->redirect = rest[1];
	}
	else
	{
		this->redirect_code = 301;
		this->redirect = rest[1];
	}
	if (redirect[0] != '/')
		std::runtime_error("redirect path must start with /");
}

void Location::validateCGI(const std::vector<std::string> &rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("invalud cgi usage");
	this->cgi_ext = rest[0];
}

void Location::validateCgiPath(const std::vector<std::string>& rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("invalid cgi path usage");
	this->cgi_path = rest[0];
}

void Location::ValidateEverything()
{
	if ((!this->cgi_ext.empty() && this->cgi_path.empty())
		|| (this->cgi_ext.empty() && !this->cgi_path.empty()))
		throw std::runtime_error("cgi path or extension not given");
	if (!this->met)
		this->methods.push_back(GET);
	if (!this->first)
		this->autoindex = false;
	if (!this->error_pages.size())
		this->error_pages = this->conf->error_pages;
	if (this->root == "")
		this->root = this->conf->root;
	if (std::find(methods.begin(),methods.end(), POST) == methods.end() && !this->upload.empty())
		throw std::runtime_error("upload path set but post is not allowed");
	if (upload[0] == '/')
		upload = conf->root + upload;
	else
		upload = root + "/" + upload;
	
}

void Location::validateUpload(const std::vector<std::string> &rest)
{
	if (rest.size() != 1)
		throw std::runtime_error("invalid usage upload");
	this->upload = rest[0];
	up = 1;
	if (upload[upload.length() - 1] == '/')
		upload.erase(upload.length() - 1);
}
