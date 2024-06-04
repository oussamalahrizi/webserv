#include "../includes/main.hpp"

RequestParser::RequestParser() {}


RequestParser::RequestParser(const std::string& input) : request(input)
{
    std::cout << this->request << std::endl;

    if (!this->checkProtocol(extractLines()[2]))
        throw std::runtime_error("invalid http protocol");
}


RequestParser::~RequestParser() {}


Method RequestParser::GetRequestType() const
{
    Method type;
    std::string sub;
    size_t i = 0;
    while (i < this->request.length() && !isspace(this->request[i]))
        i++;
    sub = this->request.substr(0, i);
    std::cout << sub << std::endl;
    if (sub == "GET")
        type = GET;
    else if (sub == "POST")
        type = POST;
    else if (sub == "DELETE")
        type = DELETE;
    else
        type = OTHER;
    return (type);
}

int RequestParser::checkProtocol(std::string& protocol)
{
    if (protocol == "HTTP/1.1")
        return (1);
    return (0);
}

std::vector<std::string> RequestParser::extractLines()
{
    std::string str = this->request.substr(0, this->request.find(CRLF));
    std::vector<std::string> lines = Utils::SplitByEach(str, " \t");
    return (lines);
}

int checkAllowedCHars(const std::string& string)
{
    std::string all("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%");
    size_t i = 0;
    while (i < string.length())
    {
        if (all.find(string[i]) == std::string::npos)
            return (0);
        i++;
    }
    return (1);
}

int RequestParser::CheckRequestFormed()
{
    // check transfer encoding and content length in post
    if (type == POST)
    {
        std::map<std::string, std::string>::const_iterator it = headers.find("Transfer-Encoding");
        if (it != headers.end() && it->second != "chunked")
        {
            this->response = new Response(501, "not imeplemented");
            return (0);
        }
        if (it == headers.end())
        {
            it = headers.find("Content-Length");
            if (it == headers.end())
            {
                this->response = new Response(400, "bad request");
                return (0);
            }
        }
    }
    // check allowed characters
    if (!checkAllowedCHars(this->uri))
    {
        this->response = new Response(400, "bad request");
        return (0);
    }
    // check uri length more that 2048
    if (this->uri.length() > 2048)
    {
        this->response = new Response(414, "Request-URI Too Long");
        return (0);
    }
    return (1);
}

Response *RequestParser::Parse()
{
    type = this->GetRequestType();
    if (type == OTHER)
        return (new Response(501, "not imeplemented"));
    size_t head_end = this->request.find(DCRLF);
    if (head_end == std::string::npos)
        throw std::runtime_error("headers have no end");
    std::vector<std::string> lines = Utils::Split(this->request.substr(0, head_end), CRLF);
    size_t i = 1;
    size_t index;
    std::string key, value;
    while (i < lines.size())
    {
        index = lines[i].find(":");
        key = Utils::Trim(lines[i].substr(0, index));
        value = Utils::Trim(lines[i].substr(index));
        this->headers[key] = value;
        i++;
    }
    this->host = headers["Host"];
    this->uri = this->extractLines()[1];
    if (!this->CheckRequestFormed())
        return (this->response);
    return (new Response(200, "OK"));
}