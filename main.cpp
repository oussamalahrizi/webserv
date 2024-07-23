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

const char* UUID::hex_chars = "0123456789abcdef";
bool UUID::seeded = false;

std::map<std::string, char> uri_encode;

void init_uri_encode()
{
    struct
    {
        std::string percent;
        char c;
    } encode [] =
    {
        {"%20", ' '}, 
        {"%21", '!'}, 
        {"%22", '"'}, 
        {"%23", '#'}, 
        {"%24", '$'}, 
        {"%25", '%'}, 
        {"%26", '&'}, 
        {"%27", '\''}, 
        {"%28", '('}, 
        {"%29", ')'}, 
        {"%2A", '*'}, 
        {"%2B", '+'}, 
        {"%2C", ','}, 
        {"%2D", '-'}, 
        {"%2E", '.'}, 
        {"%2F", '/'}, 
        {"%30", '0'}, 
        {"%31", '1'}, 
        {"%32", '2'}, 
        {"%33", '3'}, 
        {"%34", '4'}, 
        {"%35", '5'}, 
        {"%36", '6'}, 
        {"%37", '7'}, 
        {"%38", '8'}, 
        {"%39", '9'}, 
        {"%3A", ':'}, 
        {"%3B", ';'}, 
        {"%3C", '<'}, 
        {"%3D", '='}, 
        {"%3E", '>'}, 
        {"%3F", '?'}, 
        {"%40", '@'}, 
        {"%41", 'A'}, 
        {"%42", 'B'}, 
        {"%43", 'C'}, 
        {"%44", 'D'}, 
        {"%45", 'E'}, 
        {"%46", 'F'}, 
        {"%47", 'G'}, 
        {"%48", 'H'}, 
        {"%49", 'I'}, 
        {"%4A", 'J'}, 
        {"%4B", 'K'}, 
        {"%4C", 'L'}, 
        {"%4D", 'M'}, 
        {"%4E", 'N'}, 
        {"%4F", 'O'}, 
        {"%50", 'P'}, 
        {"%51", 'Q'}, 
        {"%52", 'R'}, 
        {"%53", 'S'}, 
        {"%54", 'T'}, 
        {"%55", 'U'}, 
        {"%56", 'V'}, 
        {"%57", 'W'}, 
        {"%58", 'X'}, 
        {"%59", 'Y'}, 
        {"%5A", 'Z'}, 
        {"%5B", '['}, 
        {"%5C", '\\'}, 
        {"%5D", ']'}, 
        {"%5E", '^'}, 
        {"%5F", '_'}, 
        {"%60", '`'}, 
        {"%61", 'a'}, 
        {"%62", 'b'}, 
        {"%63", 'c'}, 
        {"%64", 'd'}, 
        {"%65", 'e'}, 
        {"%66", 'f'}, 
        {"%67", 'g'}, 
        {"%68", 'h'}, 
        {"%69", 'i'}, 
        {"%6A", 'j'}, 
        {"%6B", 'k'}, 
        {"%6C", 'l'}, 
        {"%6D", 'm'}, 
        {"%6E", 'n'}, 
        {"%6F", 'o'}, 
        {"%70", 'p'}, 
        {"%71", 'q'}, 
        {"%72", 'r'}, 
        {"%73", 's'}, 
        {"%74", 't'}, 
        {"%75", 'u'}, 
        {"%76", 'v'}, 
        {"%77", 'w'}, 
        {"%78", 'x'}, 
        {"%79", 'y'}, 
        {"%7A", 'z'}, 
        {"%7B", '{'}, 
        {"%7C", '|'}, 
        {"%7D", '}'}, 
        {"%7E", '~'},
        {"", '0'},
    };
    for (size_t i = 0; encode[i].percent.length(); ++i)
        uri_encode[encode[i].percent] = encode[i].c;
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
    if (!m.size())
    {
        std::cout << "none" << std::endl;
        return; 
    }
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
    std::cout << "upload : " << loc.upload << std::endl;
    if (loc.redirect != "")
    {
        std::cout << "redirect : " << loc.redirect << std::endl;
	    std::cout << "redirect code : " << loc.redirect_code << std::endl;
    }
    if (!loc.cgi_ext.empty())
        
	if (loc.redirect != "")
    {
		std::cout << "redirect : " << loc.redirect << std::endl;
		std::cout << "redirect code: " << loc.redirect_code << std::endl;
    }
    PrintMethods(loc.methods);
    PrintMap(loc.error_pages);
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
        init_mimetypes();
        initHttpCodes();
        init_uri_encode();
		Server server(ServerConfs);
		PrintServerConfsInfo(server.confs);
		server.Start();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
