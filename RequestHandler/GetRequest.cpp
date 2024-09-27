#include "../includes/GetRequest.hpp"
#include <cstddef>
#include <string>


void GetRequest::setError(int code)
{
    this->error = code;
}

void GetRequest::get_index(std::string& name, const std::string& root)
{
    size_t i = 0;
    std::vector<std::string> index = payload.handler.index;
    if (!index.size())
        throw HttpException(404);
    while (i < index.size())
    {
        index[i] = root + index[i];
        // std::cout << "looking for file : " << index[i] << std::endl;
        if (!access(index[i].c_str(), F_OK))
        {
            if (!access(index[i].c_str(), R_OK))
            {
                name = index[i];
                return;
            }
            else
            {
                //Utils::Log("index forbidden");
                throw (HttpException(403));
            }
        }
        i++;
    }
    //Utils::Log("autoindex in index forbidden");
    throw HttpException(403);
}

std::string get_extension(const std::string& file)
{
    size_t pos = file.find_last_of(".");
    if (pos == std::string::npos || mimetype.find(file.substr(pos)) == mimetype.end())
        return ("application/octet-stream");
    
    return (mimetype.find(file.substr(pos))->second);
}

void  GetRequest::handleServeRoot()
{
    try
    {
        handleRessource(false, payload.handler.root, payload.ressource);
        if (error == 301)
            return;
        if (auto_index != NULL)
            return;
        //Utils::Log("ressource found : " + file);
        stream.open(file.c_str(), std::ios::in | std::ios::app);
        if (!stream.is_open())
        {
            perror("error open");
            //Utils::Log("open failed");
            throw HttpException(500);
        }
        if (stat(file.c_str(), &filestat) == -1)
        {
            perror("error stat");
            throw HttpException(500);
        }
        std::stringstream ss;
        ss << filestat.st_size;
        setHeaders("Content-Length", ss.str());
        setHeaders("Content-Type", get_extension(file));
        error = 200;
        //Utils::Log("we can send the response of :" + ressource);
    }
    catch (const HttpException& e)
    {
        error = e.getCode();
    }
}

int isError(int code)
{
    return (code >= 400 && code <= 511);
}

void GetRequest::setHeaders(const std::string& key, const std::string &value)
{
    headers[key] = value;
}

int GetRequest::checkAutoIndex(int autoindex)
{
    if (autoindex)
        return (1);
    std::map<std::string, std::string>::iterator it = payload.url_params.find("autoindex");
    if (it == payload.url_params.end())
        return (0);
    std::stringstream ss(it->second);
    int num = 0;
    ss >> num;
    if (num > 0)
        return (1);
    return (0);
}

void GetRequest::handleRessource(int autoindex, const std::string& root, const std::string& res)
{
    ressource = root + res;
    //Utils::Init("www/get_log.txt");
    //Utils::Log(ressource);
    // std::cout << "checking availability of :" << ressource << std::endl;
    if (ressource[ressource.length() - 1] == '/')
    {
        std::string temp = ressource.substr(0, ressource.find_last_of("/"));
        if (stat(temp.c_str(), &filestat) == -1)
        {
            if (errno == ENOENT)
            {
                //Utils::Log("stat no such file");
                throw HttpException(404);
            }
            //Utils::Log("stat failed temp" + temp);
            throw HttpException(500);
        }
        if (S_ISDIR(filestat.st_mode))
        {
            try
            {
                std::string temp_root = ressource;
                // std::cout << "ressource is a dir: " << ressource << std::endl;
                // std::cout << "index root : " << temp_root << std::endl;
                get_index(file, temp_root);
            }
            catch(const HttpException& e)
            {
                error = e.getCode();
                //Utils::Log("get index thrown : " + std::string(e.what()));
                if (autoindex)
                {
                    // std::cout << "generating auto index for : " + ressource << std::endl;
                    auto_index = new Autoindex(ressource, error, payload.ressource);
                    if (error == 500)
                    {
                        delete auto_index;
                        throw HttpException(500);
                    }
                    // std::cout << error << std::endl;
                }
                else
                {
                    //Utils::Log("index thrown and auto index allowed");
                    throw HttpException(error);
                }
            }
        }
        else
        {
            //Utils::Log("ends with / and not a dir");
            throw HttpException(404);
        }
    }
    else
    {
        if (stat(ressource.c_str(), &filestat) == -1)
        {
            if (errno == ENOENT)
            {
                //Utils::Log("stat failed no such file" + ressource);
                throw HttpException(404);
            }
            //Utils::Log("stat failed " + ressource);
            throw HttpException(500);
        }
        if (S_ISDIR(filestat.st_mode))
        {
            //Utils::Log("ressource is dir redirecting : " + res + "/");
            setHeaders("Location", payload.ressource + "/");
            error = 301;
        }
        else if (S_ISREG(filestat.st_mode))
        {
            if (!access(ressource.c_str(), F_OK))
            {
                if (res[1] == '.')
                    throw HttpException(403);
                if (!access(ressource.c_str(), R_OK))
                    file = ressource, error = 200;
                else
                {
                    //Utils::Log("regular file forbidden");
                    throw HttpException(403);
                }
            }
            else
            {
                //Utils::Log("regular file not found");
                throw HttpException(404);
            }
        }
        else
        {
            //Utils::Log("neither file or dir");
            throw HttpException(403);
        }
    }
}

GetRequest::GetRequest(data& payload, int& status_code)
{
    this->payload = payload;
    this->error = -1;
    headers_done = 0;
    auto_index = NULL;
    if (payload.serv_root)
        handleServeRoot();
    else
        handleLocation();
    status_code = error;
}

void GetRequest::handleLocation()
{
    try
    {
        // std::cout << "Location handle" << std::endl;
        size_t pos = payload.loc.path.length();
        std::string res = payload.ressource.substr(pos);
        if (res.empty())
            res = "/";
        if (res[0] != '/')
        	res = "/" + res;
        handleRessource(payload.loc.autoindex, payload.loc.root, res);
        if (error == 301)
        {
            // std::cout << payload.ressource << std::endl;
            setHeaders("Location", payload.ressource + "/");
            return;
        }
        if (auto_index != NULL)
            return;
        stream.open(file.c_str(), std::ios::in | std::ios::app);
        if (!stream.is_open())
        {
            perror("error open");
            //Utils::Log("open failed");
            throw HttpException(500);
        }
        if (stat(file.c_str(), &filestat) == -1)
            throw HttpException(500);
        std::stringstream ss;
        ss << filestat.st_size;
        setHeaders("Content-Length", ss.str());
        setHeaders("Content-Type", get_extension(file));
        error = 200;
    }
    catch (const HttpException& e)
    {
        error = e.getCode();
    }
}

GetRequest::~GetRequest()
{
    //Utils::Close();
    if (stream.is_open())
    	stream.close();
}

int GetRequest::nextChunk(std::string& chunk, int& code)
{
    chunk.clear();
    if (auto_index != NULL)
    {
        // std::cout << "sending next chunk of auto index" << std::endl;
        int res = auto_index->next_chunk(chunk, code);
        if (code == 500)
            return (1);
        if (res)
            delete auto_index;
        return (res);
    }
    if (!headers_done)
    {
        setHeaders("Connection", "close");
        if (error == 301)
        {
            chunk = "HTTP/1.1 " + http_codes[301] + CRLF;
            //Utils::Log("now setting location");
            chunk += headers.find("Location")->first + ": " + headers.find("Location")->second + CRLF;
            chunk += CRLF;
            headers_done = 1;
            return (1);
        }
        chunk = "HTTP/1.1 " + http_codes[200] + CRLF;
        std::map<std::string, std::string>::iterator it = headers.begin();
        while (it != headers.end())
        {
            chunk += it->first + ": " + it->second + CRLF;
            it++;
        }
        chunk += CRLF;
        headers_done = 1;
        return (0);
    }
    char buffer[READ_SIZE];
    stream.read(buffer, READ_SIZE);
    size_t readed = stream.gcount();
    chunk = std::string(buffer, readed);
    if (stream.eof())
    {
    	stream.close();
    	return (1);
    }
    return (0);
}