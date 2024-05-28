#include "includes/main.hpp"

void PrintMap(std::map<int, std::string>& map)
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

void PrintMethods(std::vector<Method>& m)
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

void PrintLocationInfo(Location& loc)
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

void PrintServersInfo(std::vector<Server>& servers)
{
	std::vector<Server>::iterator it = servers.begin();
	while (it != servers.end())
	{
		std::cout << "host : " << it->host << std::endl;
		std::cout << "root : " << it->root << std::endl;
		std::cout << "port : " << it->port << std::endl;
		std::cout << "index : " << std::endl;
		for (size_t i = 0; i < it->index.size(); i++)
			std::cout << it->index[i] << std::endl;
		std::cout << "server names : " << std::endl;
		for (size_t i = 0; i < it->server_names.size(); i++)
			std::cout << it->server_names[i] << std::endl;
		std::cout << "Locations : " << std::endl;
		std::map<std::string, Location>::iterator it1 = it->locations.begin();
		while (it1 != it->locations.end())
		{
			PrintLocationInfo(it1->second);
			it1++;
		}
	
		it++;
	}
	
}

int main(int ac, char **av, char **env)
{
	(void) env;
	std::vector<Server> servers;
	ConfigParser Parser;
	if (ac == 2)
	{
		try
		{
			Parser.Init(av[1], servers);
			PrintServersInfo(servers);
			return (0);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
	struct addrinfo hints;
	struct addrinfo *bind_address;
	int socket_fd;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	const char * host = "0.0.0.0";
	const char * port = "8080";
	if (getaddrinfo(host, port, &hints , &bind_address))
	{
		std::cout << "getaddinfo failed" << std::endl;
		exit(1);
	}
	
	socket_fd = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (socket_fd < 0)
	{
		std::cout << "socket failed" << std::endl;
		exit(1);
	}
	int resure = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &resure, sizeof(resure)))
	{
		std::cout << "setsocketopt failed" << std::endl;
		exit(1);
	}

	if (bind(socket_fd, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		std::cout << "bind failed" << std::endl;
		exit(1);
	}

	free(bind_address);
	listen(socket_fd, 100);

	Reactor reactor;
	reactor.AddSocket(socket_fd, new AcceptHandler(socket_fd));
	reactor.EventPool();

	return (0);
}
