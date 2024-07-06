#include "common.hpp"

class RequestHandle
{
    public:
        RequestHandle();
        virtual int nextChunk(std::string& chunk, int& status_code)
        {
            (void) chunk;
            (void) status_code;
            return (1);
        }
        virtual ~RequestHandle();
};
