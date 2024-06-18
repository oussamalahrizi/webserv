#include "../includes/Utils.hpp"

Utils::Utils() {}
Utils::~Utils() {}

std::vector<std::string> Utils::Split(const std::string& input, const std::string& delimiter)
{
	std::vector<std::string> lines;
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;

	while ((pos_end = input.find(delimiter, pos_start)) != std::string::npos)
	{
        token = input.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        lines.push_back(token);
    }
    lines.push_back(input.substr(pos_start));
    return lines;
}

std::vector<std::string> Utils::SplitByEach(const std::string& input, const std::string& delimiter)
{
    std::vector<std::string> lines;
    size_t pos_start = 0;

    for (size_t i = 0; i < input.size(); ++i)
    {
        char c = input[i];
        if (delimiter.find(c) != std::string::npos)
        {
            if (i > pos_start)
                lines.push_back(input.substr(pos_start, i - pos_start));
            pos_start = i + 1;
        }
    }
    if (pos_start < input.size())
        lines.push_back(input.substr(pos_start));
    return lines;
}

std::string Utils::Trim(const std::string& input)
{
    size_t start = 0;
    while (start < input.length() && std::isspace(input[start]))
        start++;
    size_t end = input.length();
    while (end > start && std::isspace(input[end - 1]))
        end--;
    return input.substr(start, end - start);
}

int Utils::CheckNumeric(const std::string& value, size_t len)
{
    if (value.length() != len || value.length() < len)
        return 0;
    size_t i = 0;
    while (i < len)
    {
        if (!std::isdigit(value[i]))
            return (0);
        i++;
    }
    return (1);
}

int Utils::findServer(const std::map<std::string, std::string>& hosts,
										ServerConf& s)
{
    std::map<std::string, std::string>::const_iterator it = hosts.find(s.host);
    if (it != hosts.end() && it->second == s.port)
        return (1);
    return (0);
}

std::string	Utils::getErrorcode(int error_code)
{
    std::string statusMessage = http_codes[error_code];
    std::stringstream ss;
    ss << error_code;
    std::string htmlTemplate = 
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>Error " + ss.str() + "</title>\n"
    "    <style>\n"
    "        body {\n"
    "            font-family: Arial, sans-serif;\n"
    "            text-align: center;\n"
    "            padding: 50px;\n"
    "            background-color: #f2f2f2;\n"
    "        }\n"
    "        .container {\n"
    "            max-width: 600px;\n"
    "            margin: 0 auto;\n"
    "            background: #fff;\n"
    "            padding: 20px;\n"
    "            border-radius: 10px;\n"
    "            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n"
    "        }\n"
    "        h1 {\n"
    "            font-size: 48px;\n"
    "            margin: 0;\n"
    "            color: #333;\n"
    "        }\n"
    "        p {\n"
    "            font-size: 18px;\n"
    "            color: #666;\n"
    "        }\n"
    "        a {\n"
    "            color: #007BFF;\n"
    "            text-decoration: none;\n"
    "        }\n"
    "        a:hover {\n"
    "            text-decoration: underline;\n"
    "        }\n"
    "     </style>\n"
    "</head>\n"
    "<body>\n"
    "   <div class=\"container\">\n"
    "       <h1>Error " + ss.str() + "</h1>\n"
    "       <p>" + statusMessage + "</p>\n"
    "       <p><a href=\"/\">Go to Homepage</a></p>\n"
    "   </div>\n"
    "</body>\n"
"</html>";

    return htmlTemplate;
}