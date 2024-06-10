#include "includes/common.hpp"
#include "includes/ServerConf.hpp"
#include "includes/HttpExceptions.hpp"
#include "includes/HttpHandler.hpp"

int checkHeaderEnd(std::string& request, size_t &index)
{
    if ((index = request.find(DCRLF)) != std::string::npos)
        return (1);
    return (0);
}

std::map<std::string, std::string> extractHeaders(std::string request)
{
    std::map<std::string, std::string> headers;
    std::vector<std::string> lines = Utils::Split(request, CRLF);
    size_t i = 1, index;
    std::string key, value;
    while (i < lines.size())
    {
        index = lines[i].find(": ");
        key = Utils::Trim(lines[i].substr(0, index));
        value = Utils::Trim(lines[i].substr(index + 2));
        headers[key] = value;
        i++;
    }
    return (headers);
}

int check_protocol(std::string line)
{
    std::vector<std::string> splited = Utils::SplitByEach(line, " ");
    if (splited[2] != "HTTP/1.1")
        return (0);
    return (1);
}

Method getRequestType(std::string line)
{
    std::vector<std::string> splited = Utils::SplitByEach(line, " ");
    if (splited[0] == "GET")
        return (GET);
    if (splited[0] == "POST")
        return (POST);
    if (splited[0] == "DELETE")
        return (DELETE);
    return (OTHER);
}

void ValidateTransfer(std::map<std::string, std::string>& headers, Method type)
{
    // ignore this check if the method is not post
    if (type != POST)
        return;
    /*
        if =>
        Transfer-Encoding not exist
        Content-Length not exist
        The method is Post
    */
    std::map<std::string, std::string>::iterator it = headers.find("Transfer-Encoding");
    if (it == headers.end())
    {
        it = headers.find("Content-Length");
        if (it == headers.end())
            throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
    }
    /*
        if => Transfer-Encoding header
        exist and is different to “chunked”
    */
    else if (it->second != "chunked")
        throw HttpException(http_codes.find(501)->first, http_codes.find(501)->second);
}

void checkAllowedCHars(std::string& uri)
{
    std::string allow = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    size_t i = 0;
    while (i < uri.length())
    {
        if (allow.find(uri[i]) == std::string::npos)
            throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
        i++;
    }
}

void is_req_well_formed(std::map<std::string, std::string>& headers, Method type, std::string& uri)
{
    ValidateTransfer(headers, type);
    if (uri.length() > 2048)
        throw HttpException(http_codes.find(414)->first, http_codes.find(414)->second);
    checkAllowedCHars(uri);
}

data Parse(std::string request)
{
    data result;
    size_t header_end = -1;
    if (!checkHeaderEnd(request, header_end))
        throw HttpException(http_codes.find(400)->first, http_codes.find(400)->second);
    if (!check_protocol(request.substr(0, request.find_first_of(CRLF))))
        throw HttpException(http_codes.find(505)->first, http_codes.find(505)->second);
    Method type = getRequestType(request.substr(0, request.find_first_of(CRLF)));
    if (type == OTHER)
        throw HttpException(http_codes.find(501)->first, http_codes.find(501)->second);
    std::map<std::string, std::string> headers = extractHeaders(request.substr(0, header_end));
}