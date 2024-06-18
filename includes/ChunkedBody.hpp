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
        int chunk_size;
        std::string body;
        std::string sub;
        int body_fd;
        
        ChunkedBody();
        ChunkedBody(const ChunkedBody& other);
        ChunkedBody& operator=(const ChunkedBody& other);
    public:
        ChunkedBody(data& payload);
        int transfer(const std::string& data);
        ~ChunkedBody();
        
};
