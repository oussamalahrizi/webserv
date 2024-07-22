
# include "../includes/DeleteRequest.hpp"


DeleteRequest::DeleteRequest(data &payload, int &status_code)
{
    // std::cout << "path : " << payload.ressource << std::endl;
    std::string res = payload.ressource;
    std::string newLocation;
    newLocation = res.substr(payload.loc.path.length());
    if (newLocation.empty())
        newLocation = "/";
    res = payload.loc.root + newLocation;
    try
    {
        this->checkPath(res, status_code);
    }
    catch (HttpException &e)
    {
        status_code = e.getCode();
    }
    // std::cout << "new res : " << res << std::endl;   
    // status_code = 501;
}

void DeleteRequest::checkPath(const std::string &path, int &status_code)
{
    struct stat path_stat;
    static int check = 0;
    std::string tmppath;
    std::cout << "path is : " << path << std::endl;
    if (stat(path.c_str(), &path_stat) == -1)
    {
        if (errno == ENOENT)
        {
            std::cout << "HERE" << std::endl;
            throw HttpException(404);
        }
        throw HttpException(500);
    }
    if (S_ISREG(path_stat.st_mode))
    {
        if (!access(path.c_str(), W_OK))
        {
            if (unlink(path.c_str()))
            {
                throw HttpException(500);
            }
            status_code = 204;
            return ;
        }
        throw HttpException(403);
    }
    if (S_ISDIR(path_stat.st_mode) && path.at(path.length() - 1) != '/' && check == 0)
        throw HttpException(409);
    else if (S_ISDIR(path_stat.st_mode))
    {
        check = 1;
        DIR *dir = opendir(path.c_str());
        if (!dir)
            throw HttpException(500);
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
                continue ;
            tmppath = path + "/" + entry->d_name;
            for (size_t i = 0; i < tmppath.length() - 1; i++)
            {
                if (tmppath[i] == '/' && tmppath[i + 1] == '/')
                {
                    tmppath.erase(i, 1);
                    i--;
                }
            }
            std::cout << "tmp: " << tmppath << std::endl;
            checkPath(tmppath , status_code);
        }
        closedir(dir);
        std::remove(path.c_str());
    }
}

DeleteRequest::~DeleteRequest() {}

int DeleteRequest::nextChunk(std::string& chunk, int& code)
{
    (void) code;
    chunk = "HTTP/1.1 " + http_codes[204] + " " + CRLF;
    chunk += "Connection: close\r\n\r\n";
    return (1);
}
