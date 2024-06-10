#include "includes/common.hpp"
#include "includes/Location.hpp"
#include "includes/Server.hpp"
#include "includes/ServerConf.hpp"
#include "includes/ConfigParser.hpp"

std::map<std::string, std::string> mimetype;
void init_mimetypes()
{
    struct
    {
        std::string type;
        std::string ext;
    } exts_struct[] = {
        {"audio/aac", ".aac"},
        {"application/x-abiword", ".abw"},
        {"image/apng", ".apng"},
        {"application/x-freearc", ".arc"},
        {"image/avif", ".avif"},
        {"video/x-msvideo", ".avi"},
        {"application/vnd.amazon.ebook", ".azw"},
        {"application/octet-stream", ""},
        {"image/bmp", ".bmp"},
        {"application/x-bzip", ".bz"},
        {"application/x-bzip2", ".bz2"},
        {"application/x-cdf", ".cda"},
        {"application/x-csh", ".csh"},
        {"text/css", ".css"},
        {"text/csv", ".csv"},
        {"application/msword", ".doc"},
        {"application/vnd.openxmlformats-officedocument.wordprocessingml.document", ".docx"},
        {"application/vnd.ms-fontobject", ".eot"},
        {"application/epub+zip", ".epub"},
        {"application/gzip", ".gz"},
        {"image/gif", ".gif"},
        {"text/html", ".html"},
        {"image/vnd.microsoft.icon", ".ico"},
        {"text/calendar", ".ics"},
        {"application/java-archive", ".jar"},
        {"image/jpeg", ".jpeg"},
        {"image/jpeg", ".jpg"},
        {"text/javascript", ".js"},
        {"application/json", ".json"},
        {"application/ld+json", ".jsonld"},
        {"audio/midi", ".mid"},
        {"audio/midi", ".midi"},
        {"text/javascript", ".mjs"},
        {"audio/mpeg", ".mp3"},
        {"video/mp4", ".mp4"},
        {"video/mpeg", ".mpeg"},
        {"application/vnd.apple.installer+xml", ".mpkg"},
        {"application/vnd.oasis.opendocument.presentation", ".odp"},
        {"application/vnd.oasis.opendocument.spreadsheet", ".ods"},
        {"application/vnd.oasis.opendocument.text", ".odt"},
        {"audio/ogg", ".oga"},
        {"video/ogg", ".ogv"},
        {"application/ogg", ".ogx"},
        {"audio/opus", ".opus"},
        {"font/otf", ".otf"},
        {"image/png", ".png"},
        {"application/pdf", ".pdf"},
        {"application/x-httpd-php", ".php"},
        {"application/vnd.ms-powerpoint", ".ppt"},
        {"application/vnd.openxmlformats-officedocument.presentationml.presentation", ".pptx"},
        {"application/vnd.rar", ".rar"},
        {"application/rtf", ".rtf"},
        {"application/x-sh", ".sh"},
        {"image/svg+xml", ".svg"},
        {"application/x-tar", ".tar"},
        {"image/tiff", ".tif"},
        {"image/tiff", ".tiff"},
        {"video/mp2t", ".ts"},
        {"font/ttf", ".ttf"},
        {"text/plain", ".txt"},
        {"application/vnd.visio", ".vsd"},
        {"audio/wav", ".wav"},
        {"audio/webm", ".weba"},
        {"video/webm", ".webm"},
        {"image/webp", ".webp"},
        {"font/woff", ".woff"},
        {"font/woff2", ".woff2"},
        {"application/xhtml+xml", ".xhtml"},
        {"application/vnd.ms-excel", ".xls"},
        {"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", ".xlsx"},
        {"application/xml", ".xml"},
        {"application/vnd.mozilla.xul+xml", ".xul"},
        {"application/zip", ".zip"},
        {"video/3gpp; audio/3gpp", ".3gp"},
        {"video/3gpp2; audio/3gpp2", ".3g2"},
        {"application/x-7z-compressed", ".7z"},
        {"", ""},
    };

    for (size_t i = 0; exts_struct[i].type.length(); ++i)
    {
        mimetype[exts_struct[i].type] = exts_struct[i].ext;
        mimetype[exts_struct[i].ext
        ] = exts_struct[i].type;
    }
}

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
        init_mimetypes();
		Server server(ServerConfs);
		server.Start();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
