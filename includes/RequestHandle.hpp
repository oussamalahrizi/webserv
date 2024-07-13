#include "common.hpp"

class RequestHandle
{
    public:
        RequestHandle() {}
        virtual int nextChunk(std::string& chunk) = 0;
        virtual ~RequestHandle() {}
};
