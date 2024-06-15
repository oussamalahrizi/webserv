#pragma once

#include "common.hpp"
#include "HttpHandler.hpp"


typedef enum s_state
{
    SIZE,
    DATA,
    END
} t_state;


class ChunkedBody
{
    private:
        t_state state;
        unsigned long long total;
        int chunk_size;
        std::string body;
        std::string sub;

    public:
        ChunkedBody(data& payload);
        ~ChunkedBody();
        
};
