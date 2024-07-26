
# pragma once

# include "HttpHandler.hpp"
# include "RequestHandle.hpp"
# include "common.hpp"

enum 
{
	DEL_HEAD,
	DEL_FILE,
	DEL_TAIL
};

class DeleteRequest : public RequestHandle
{
    private:
        data payload;
        std::map<std::string, std::string> headers;
        std::map<std::string, int> file_status;
        std::map<std::string, int>::iterator it_res;
        int check;
        int state;
        int headers_done;
    private:
    	void setFileStatus(const std::string& path, int code);
     	std::string getFileXml(const std::string& path);
      	std::string getHead() const;
      	std::string getTail() const;
       	std::string getContentLength();
     
    public:
        DeleteRequest(data &paylead, int &status_code);
        ~DeleteRequest();
        void checkPath(const std::string &path, int &stats_code);
        int nextChunk(std::string& chunk, int& code);
};