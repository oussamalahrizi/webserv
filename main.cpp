#include "includes/main.hpp"

void PrintMap(std::map<int, std::string> &map)
{
	std::cout << "error pages : " << std::endl;
	std::map<int, std::string>::const_iterator it = map.begin();
	while (it != map.end())
	{
		std::cout << "key : " << it->first << std::endl;
		std::cout << "value : " << it->second << std::endl;
		it++;
	}
}

void PrintMethods(std::vector<Method> &m)
{
	std::cout << "methods : " << std::endl;
	for (size_t i = 0; i < m.size(); i++)
	{
		if (m[i] == GET)
			std::cout << "GET" << std::endl;
		else if (m[i] == POST)
			std::cout << "POST" << std::endl;
		else if (m[i] == DELETE)
			std::cout << "DELETE" << std::endl;
	}
}

void PrintLocationInfo(Location &loc)
{
	std::cout << "path : " << loc.path << std::endl;
	std::cout << "root : " << loc.root << std::endl;
	std::cout << "autoindex : " << loc.autoindex << std::endl;
	if (loc.error_pages.size())
		PrintMap(loc.error_pages);
	PrintMethods(loc.methods);
	if (loc.redirect != "")
		std::cout << "redirect : " << loc.redirect << std::endl;
	if (loc.nestedLocations.size())
	{
		std::cout << "nested locations : " << std::endl;
		for (size_t i = 0; i < loc.nestedLocations.size(); i++)
			PrintLocationInfo(loc.nestedLocations[i]);
	}
}

void PrintServerConfsInfo(std::vector<ServerConf> &ServerConfs)
{
	std::vector<ServerConf>::iterator it = ServerConfs.begin();
	while (it != ServerConfs.end())
	{
		std::cout << "host : " << it->host << std::endl;
		std::cout << "root : " << it->root << std::endl;
		std::cout << "port : " << it->port << std::endl;
		std::cout << "index : " << std::endl;
		for (size_t i = 0; i < it->index.size(); i++)
			std::cout << it->index[i] << std::endl;
		std::cout << "Server names : " << std::endl;
		for (size_t i = 0; i < it->Server_names.size(); i++)
			std::cout << it->Server_names[i] << std::endl;
		if (it->locations.size() > 0)
		{
			std::cout << "Locations : " << std::endl;
			std::map<std::string, Location>::iterator it1 = it->locations.begin();
			while (it1 != it->locations.end())
			{
				PrintLocationInfo(it1->second);
				it1++;
			}
		}
		std::cout << "------------------" << std::endl;
		it++;
	}
}

int main(int ac, char **av, char **env)
{
	(void)env;
	std::vector<ServerConf> ServerConfs;
	ConfigParser Parser;
	if (ac >= 2)
	{
		std::cerr << "wrong usage" << std::endl;
		return (1);
	}
	try
	{
		if (ac == 2)
			Parser.Init(av[1], ServerConfs);
		else
			Parser.Init("conf.d/server.conf", ServerConfs);
		PrintServerConfsInfo(ServerConfs);
		Server server(ServerConfs);
		server.Start();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
