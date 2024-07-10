#include "../includes/GetRequest.hpp"


void GetRequest::setError(int code)
{
    this->error = code;
}

std::string get_index(std::vector<std::string>& index, std::string& dir)
{

}

void  GetRequest::handleServeRoot()
{
    if (payload.ressource == "/")
    {
        int i = 0;
        while (i < payload.handler.index.size())
        {
            if (!access((payload.handler.root + payload.handler.index[i]).c_str(), F_OK | R_OK))
            {
                this->file = payload.handler.root + payload.handler.index[i];
                return;
            }
            i++;
        }
        if (i == payload.handler.index.size())
            return setError(403);
    }
    std::string res = payload.handler.root + payload.ressource;
    if (res[res.length() - 1] == '/')
    {
        // test for dir
        struct stat dbuf;
        if (stat(res.c_str(), &dbuf) == -1)
            return setError(500);
        if (dbuf.st_mode && S_IFREG) // ressource is a file
        {
            this->file = payload.ressource;
            return;
        }
        return setError(403);
        return;
    }
    if (access(res.c_str(), F_OK) != 0)
        return setError(404);
    if (access(res.c_str(), R_OK) != 0)
        return setError(403);
    
 // ressource is probably a dir and autoindex is off by default here
}

int isError(int code)
{
    return (code >= 400 && code <= 511);
}

GetRequest::GetRequest(data& payload)
{
    this->payload = payload;
    this->error = -1;
    if (payload.serv_root)
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
    while (1);
}

