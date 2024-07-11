#include "../includes/GetRequest.hpp"


void GetRequest::setError(int code)
{
    this->error = code;
}

std::string get_index(std::vector<std::string>& index, const std::string& dir)
{
    std::string res("");
    std::string temp;

    int i = 0;
    while (i < index.size())
    {
        temp = dir + "/" + index[i];
        if (!access(temp.c_str(), F_OK | R_OK))
        {
            res = temp;
            break;
        }
        i++;
    }
    return res;
}

void GetRequest::testDir(const std::string& res)
{
    struct stat dbuf;

    if (stat(res.c_str(), &dbuf) == -1)
    {
        Utils::Log("stat failed, testing dir");
        return setError(500);
    }
    if (dbuf.st_mode & S_IFDIR)
    {
        Utils::Log("ressource is a dir, looking for index");
        std::string temp = get_index(payload.handler.index, res);
        if (temp.empty())
        {
            Utils::Log("ressource dir has no index : 403");
            return setError(403);
        }
        Utils::Log("found index inside the dir, testing permission");
        if (access(temp.c_str(), F_OK | R_OK))
        {

        }
        this->file = temp;
        return setError(200);
    }
}

void GetRequest::testFile(const std::string& res)
{
    struct stat dbuf;

    if (stat(res.c_str(), &dbuf) == -1)
    {
        Utils::Log("stat failed, testing file");
        return setError(500);
    }
}

void  GetRequest::handleServeRoot()
{
    if (payload.ressource == "/")
    {
        Utils::Log("ressource is straight root, looking for index");
        std::string res = get_index(payload.handler.index, payload.handler.root);
        if (res.empty())
        {
            Utils::Log("index not found returning forbidden");
            return setError(403);
        }
        this->file = res;
        Utils::Log("index found");
        return setError(200);
    }
    std::string res = payload.handler.root + payload.ressource;
    Utils::Log("ressource to serv from root is : " + res);
    if (res[res.length() - 1] == '/')
    {
        Utils::Log("ressource has trailing slash");
        // remove trailing /
        res.erase(res.length() - 1);
        testDir(res);
        Utils::Log("request has trailing / correct url with location : " + res);
        return setError(301);
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
    Utils::Init("getLog.txt");
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

GetRequest::~GetRequest()
{
    Utils::Close();
}
