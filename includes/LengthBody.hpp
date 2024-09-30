#pragma once


#include "common.hpp"
#include "HttpHandler.hpp"

class LengthBody
{
    private:
        unsigned long long cl;
        unsigned long long max_size;
        unsigned long long total;
        std::ofstream file_stream;
        LengthBody();
        LengthBody(const LengthBody& other);
        LengthBody& operator=(const LengthBody& other);
    public:
        LengthBody(data& payload);
        int transfer(const std::string& buffer);
        size_t getLength() { return cl;}
        ~LengthBody();
};
