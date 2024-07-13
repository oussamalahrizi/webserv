#include "../includes/GetRequest.hpp"


void GetRequest::setError(int code)
{
    this->error = code;
}

std::string get_index(const std::vector<std::string>& index, const std::string& res, int& error)
{
    std::string temp = "";
    if (!index.size())
        return (temp);
    int i = 0;
    while (i < index.size())
    {
        if (access(index[i].c_str(), F_OK))
        {
            i++;
            continue;
        }
        if (access(index[i].c_str(), R_OK))
        {
            error = 403;
            return "";
        }
        temp = index[i];
        error = 200;
        return (temp);
    }
    error = 404;
    return("");
}

void  GetRequest::handleServeRoot()
{
    res = payload.handler.root + payload.ressource;
    Utils::Log("ressource is : " + res);
    if (res == "/")
    {
    }
}

int isError(int code)
{
    return (code >= 400 && code <= 511);
}

GetRequest::GetRequest(data& payload)
{
    Utils::Init("getLog.txt");
    this->payload = payload;
    this->error = -1;
=    if (payload.serv_root)
        handleServeRoot();
    // if (isError(error)) // get file name from error page or generate it if doesnt exist
    // {
    //     // if the error page is provided
    //     //then we can only return the name of the file else
    //     // we should generate it so somehow
    //     //we should store the generated error page in a temp file
    // }
    this->fd = open(this->file.c_str(), O_RDONLY);
    if (fd < 0)
        setError(500);
}

GetRequest::~GetRequest()
{
    Utils::Close();
}

int GetRequest::nextChunk(std::string& chunk)
{
    (void) chunk;
    // handle error page if necessary
    return (1);
}