
# pragma once

# include "HttpHandler.hpp"
# include "RequestHandle.hpp"
# include "common.hpp"

class DeleteRequest : public RequestHandle
{
    private:
        data payload;
        std::map<std::string, std::string> headers;
        int check;
    public:
        DeleteRequest(data &paylead, int &status_code);
        ~DeleteRequest();
        void checkPath(const std::string &path, int &stats_code);
        int nextChunk(std::string& chunk, int& code);
};