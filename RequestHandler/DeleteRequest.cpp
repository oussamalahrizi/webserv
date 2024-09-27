
# include "../includes/DeleteRequest.hpp"

DeleteRequest::DeleteRequest(data &payload, int &status_code) : payload(payload)
{
    headers_done = 0;
    std::string res = payload.ressource;
    std::string newLocation;
    newLocation = res.substr(payload.loc.path.length());
    if (newLocation.empty())
        newLocation = "/";
    if (newLocation[0] != '/')
    	newLocation = "/" + newLocation;
    res = payload.loc.root + newLocation;
    try
    {
        check = 0;
        this->checkPath(res, status_code);
    }
    catch (HttpException &e)
    {
    	// has to be 409
        status_code = e.getCode();
        return;
    }
    if (file_status.size() > 1)
    	status_code = 207;
    else if (file_status.size() == 1) 
    	status_code = file_status.begin()->second;
    else
    {
    	std::cout << "file status size wrong : " << file_status.size() << std::endl;
     	status_code = 500;
    }
}

void DeleteRequest::setFileStatus(const std::string& path, int code)
{
	std::string ressource = path.substr(payload.loc.root.length());
	ressource = payload.loc.path + ressource;
	file_status[ressource] = code;
}

void DeleteRequest::checkPath(const std::string &path, int &status_code)
{
    struct stat path_stat;
    std::string tmppath;
    if (stat(path.c_str(), &path_stat) == -1)
    {
        if (errno == ENOENT)
        	return setFileStatus(path, 404);
        return setFileStatus(path, 500);
    }
    if (S_ISREG(path_stat.st_mode))
    {
        if (!access(path.c_str(), W_OK))
        {
            if (unlink(path.c_str()))
                return setFileStatus(path, 500);
            return setFileStatus(path, 204);
        }
        return setFileStatus(path, 403);
    }
    if (S_ISDIR(path_stat.st_mode) && path.at(path.length() - 1) != '/' && check == 0)
        throw HttpException(409);
    else if (S_ISDIR(path_stat.st_mode))
    {
        check = 1;
        DIR *dir = opendir(path.c_str());
        if (!dir)
        {
        	if (errno == EACCES)
         		return setFileStatus(path, 403);
           	if (errno == ENOENT)
         		return setFileStatus(path, 404);
            return setFileStatus(path, 500);
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
        	// skipping . and .. to avoid infinite recursion
            if (!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
                continue ;
            tmppath = path + "/" + entry->d_name;
            // remove consecutive / for safety
            for (size_t i = 0; i < tmppath.length() - 1; i++)
            {
                if (tmppath[i] == '/' && tmppath[i + 1] == '/')
                {
                    tmppath.erase(i, 1);
                    i--;
                }
            }
            checkPath(tmppath , status_code);
        }
        closedir(dir);
        if (std::remove(path.c_str()) == -1)
        {
        	if (errno == EACCES || errno == ENOTEMPTY)
         		return setFileStatus(path, 403);
           	return setFileStatus(path, 500);
        }
        return setFileStatus(path, 204);
    }
}

DeleteRequest::~DeleteRequest() {}

std::string DeleteRequest::getFileXml(const std::string& path)
{
	std::map<std::string, int>::iterator it = file_status.find(path);
	if (it == file_status.end())
		return "";
	
	std::stringstream ss;
	ss << "<d:response>";
	ss << "<d:href>" + it->first +"</d:href>";
	ss << "<d:status>HTTP/1.1 "+http_codes[it->second]+"</d:status>";
	ss << "</d:response>";
	return ss.str();
}
std::string DeleteRequest::getHead() const
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	ss << "<d:multistatus xmlns:d=\"DAV:\">";
	return ss.str();
}
std::string DeleteRequest::getTail() const
{
	return "</d:multistatus>";
}

std::string DeleteRequest::getContentLength()
{
	size_t cl;

	cl = getHead().length();
	std::map<std::string, int>::iterator it = file_status.begin();
	while (it != file_status.end())
	{
		cl += getFileXml(it->first).length();
		it++;
	}
	cl += getTail().length();
	std::stringstream ss;
	ss << cl;
	return ss.str();
}

int DeleteRequest::nextChunk(std::string& chunk, int& code)
{
	chunk.clear();
    if (!headers_done)
    {
	   	chunk = "HTTP/1.1 " + http_codes[code] + " " + CRLF;
		if (code == 207)
		{	
			chunk += "Content-Type: application/xml; charset=\"utf-8\"";
			chunk += CRLF;
			chunk += "Content-Length: " + getContentLength() + CRLF;
			state = DEL_HEAD;
			it_res = file_status.begin();
		}
	    chunk += "Connection: close";
		chunk += DCRLF;
		headers_done = 1;
		if (code == 207)
			return (0);
		return (1);
    }
    switch (state)
    {
    	case DEL_HEAD:
     		chunk = getHead();
       		state = DEL_FILE;
         	return (0);
         	break;
        case DEL_FILE:
        	chunk = getFileXml(it_res->first);
         	it_res++;
          	if (it_res == file_status.end())
           	{
            	state = DEL_TAIL;
             	return (0);
            }
            break;
        case DEL_TAIL:
        	chunk = getTail();
         	return (1);
          	break;
        default: break;
    }
    return (0);
}
