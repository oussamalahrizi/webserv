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

std::map<int, std::string> http_codes;
void initHttpCodes()
{
    struct
    {
        int code;
        std::string message;
    } exts_struct[] = {
        {100, "100 Continue"},
        {101, "101 Switching Protocols"},
        {102, "102 Processing"},
        {103, "103 Early Hints"},

        // 2xx Success
        {200, "200 OK"},
        {201, "201 Created"},
        {202, "202 Accepted"},
        {203, "203 Non-Authoritative Information"},
        {204, "204 No Content"},
        {205, "205 Reset Content"},
        {206, "206 Partial Content"},
        {207, "207 Multi-Status"},
        {208, "208 Already Reported"},
        {226, "226 IM Used"},

        // 3xx Redirection
        {300, "300 Multiple Choices"},
        {301, "301 Moved Permanently"},
        {302, "302 Found"},
        {303, "303 See Other"},
        {304, "304 Not Modified"},
        {305, "305 Use Proxy"},
        {306, "306 Switch Proxy"},
        {307, "307 Temporary Redirect"},
        {308, "308 Permanent Redirect"},

        // 4xx Client errors
        {400, "400 Bad Request"},
        {401, "401 Unauthorized"},
        {402, "402 Payment Required"},
        {403, "403 Forbidden"},
        {404, "404 Not Found"},
        {405, "405 Method Not Allowed"},
        {406, "406 Not Acceptable"},
        {407, "407 Proxy Authentication Required"},
        {408, "408 Request Timeout"},
        {409, "409 Conflict"},
        {410, "410 Gone"},
        {411, "411 Length Required"},
        {412, "412 Precondition Failed"},
        {413, "413 Payload Too Large"},
        {414, "414 URI Too Long"},
        {415, "415 Unsupported Media Type"},
        {416, "416 Range Not Satisfiable"},
        {417, "417 Expectation Failed"},
        {418, "418 I'm a teapot"},
        {421, "421 Misdirected Request"},
        {422, "422 Unprocessable Entity"},
        {423, "423 Locked"},
        {424, "424 Failed Dependency"},
        {425, "425 Too Early"},
        {426, "426 Upgrade Required"},
        {428, "428 Precondition Required"},
        {429, "429 Too Many Requests"},
        {431, "431 Request Header Fields Too Large"},
        {451, "451 Unavailable For Legal Reasons"},

        // 5xx Server errors
        {500, "500 Internal Server Error"},
        {501, "501 Not Implemented"},
        {502, "502 Bad Gateway"},
        {503, "503 Service Unavailable"},
        {504, "504 Gateway Timeout"},
        {505, "505 HTTP Version Not Supported"},
        {506, "506 Variant Also Negotiates"},
        {507, "507 Insufficient Storage"},
        {508, "508 Loop Detected"},
        {510, "510 Not Extended"},
        {511, "511 Network Authentication Required"},
        {0, ""},
    };

    for (size_t i = 0; exts_struct[i].message.length(); ++i)
        http_codes[exts_struct[i].code] = exts_struct[i].message;
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
		std::cout << "client max body size : " << it->max_body_size << " bytes" << std::endl;
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
        initHttpCodes();
		Server server(ServerConfs);
		server.Start();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
