#include "../includes/Autoindex.hpp"
#include "../includes/HttpExceptions.hpp"

Autoindex::Autoindex(const std::string& path, int& code, const std::string& root) : current_index(0), dir_path(path), href(root)
{
	dir = opendir(dir_path.c_str());
	if (!dir)
	{
		std::cout << "opendir failed " << path << std::endl;
		code = 500;
	}
	else
	{
		generateFileList();
		headers = 0;
		code = error = 200;
	}
}

void Autoindex::generateFileList()
{
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (strncmp(entry->d_name, ".", 1)) // exclude ".." and "."
            fileList.push_back(entry->d_name);
	}
	std::sort(fileList.begin(), fileList.end());
	std::cout << "file list generated " << std::endl;
}

std::string Autoindex::getFileInfo(const std::string& filename)
{
	struct stat fileStat;
	std::string path = dir_path + "/" + filename;
	if (stat(path.c_str(), &fileStat) == -1)
	{
		std::cout << "stat failed autoindex : " + path << std::endl;
		throw HttpException(500);
	}
	std::stringstream ss;
	std::string name = filename;
	if (filename.length() > 50)
		name = filename.substr(0, 51) + "...";
	
	std::string tmp = href;
	if (href[href.length() - 1] == '/')
		tmp.erase(tmp.length() - 1);
	ss << "<tr>";
	ss << "<td><a href=\"" << tmp + "/" + name << (S_ISDIR(fileStat.st_mode) ? "/" : "") << "\">" 
		<< name << (S_ISDIR(fileStat.st_mode) ? "/" : "") << "</a></td>";
	ss << "<td>" << fileStat.st_size << "</td>";
	
	char timeStr[20];
	strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&fileStat.st_mtime));
	ss << "<td>" << timeStr << "</td>";
	ss << "</tr>\n";
	return ss.str();
}

std::string Autoindex::getHead() const
{
	std::stringstream ss;
	ss << "<!DOCTYPE html>\n"
		<< "<html lang=\"en\">\n"
		<< "<head>\n"
		<< "    <meta charset=\"UTF-8\">\n"
		<< "    <title>Index of " << href << "</title>\n"
		<< "    <style>\n"
		<< "        body { font-family: Arial, sans-serif; }\n"
		<< "        table { border-collapse: collapse; width: 100%; }\n"
		<< "        th, td { text-align: left; padding: 8px; }\n"
		<< "        tr:nth-child(odd) { background-color: #f2f2f2; }\n"
		<< "        th { background-color: #4CAF50; color: white; }\n"
		<< "    </style>\n"
		<< "</head>\n";
	return ss.str();
}

std::string Autoindex::getTitle() const
{
	std::stringstream ss;
	ss << "<body>\n"
		<< "    <h1>Index of " << href << "</h1>\n"
		<< "    <table>\n"
		<< "        <tr>\n"
		<< "            <th>Name</th>\n"
		<< "            <th>Size</th>\n"
		<< "            <th>Last Modified</th>\n"
		<< "        </tr>\n"
		<< "        <tr>\n"
		<< "            <td><a href=\"..\">../</a></td>\n"
		<< "        </tr>\n";
	return ss.str();
}

std::string Autoindex::getTail() const
{
	return "</table>\n</body>\n</html>\n";
}

int Autoindex::getContentLength()
{
	int total;

	total = getHead().length();
	total += getTitle().length();
	std::vector<std::string>::iterator it = fileList.begin();
	while (it != fileList.end())
	{
		total += getFileInfo(*it).length();
		it++;
	}
	total += getTail().length();
	return total;
}

int Autoindex::next_chunk(std::string& chunk, int& code)
{
	try
	{
		if (!headers)
		{
			chunk = "HTTP/1.1 " + http_codes[200] + CRLF;
			chunk += "Content-Type: " + mimetype[".html"] + CRLF;
			chunk += "Connection: close";
			chunk += CRLF;
			std::stringstream ss;
			ss << getContentLength();
			chunk += "Content-Length: " + ss.str() + CRLF;
			chunk += CRLF;
			headers = 1;
			code = 200;
			state = HEAD;
			return (0);
		}
		switch (state)
		{
			case HEAD:
				chunk = getHead();
				state = TITLE;
				break;
			case TITLE:
				chunk = getTitle();
				state = CONTENT;
				break;
			case CONTENT:
				chunk = getFileInfo(fileList[current_index]);
				current_index++;
				std::cout << "vector size : " << fileList.size() << std::endl;
				std::cout << "current index : " << current_index << std::endl;
				if (fileList.size() == (size_t)current_index)
				{
					state = TAIL;
					return (0);
				}
				break;
			case TAIL:
				chunk = getTail(); break;
			default : break;
		}
		if (state == TAIL)
			return (1);
		return (0);
	}
	catch (const std::exception& e)
	{
		code = 500;
		return (1);
	}
}

Autoindex::~Autoindex()
{
	closedir(dir);
}