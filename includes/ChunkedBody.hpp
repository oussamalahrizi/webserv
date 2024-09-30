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
        unsigned long long max_size;
        size_t chunk_size;
        std::string body;
        std::string sub;
        std::ofstream file_stream;
        ChunkedBody();
        ChunkedBody(const ChunkedBody& other);
    public:
        ChunkedBody(data& payload);
        int transfer(const std::string& buffer);
        ~ChunkedBody();
        
};
