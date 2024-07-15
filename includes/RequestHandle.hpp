#include "common.hpp"

class RequestHandle
{
    public:
        RequestHandle() {}
        virtual int nextChunk(std::string& chunk, int& code) = 0;
        virtual ~RequestHandle() {}
};
