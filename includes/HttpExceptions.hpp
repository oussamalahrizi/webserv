#pragma once

#include "common.hpp"

class HttpException : public std::exception
{
    private:
        int code;
        std::string message;
    public:
        HttpException(): code(0), message("") {}
        HttpException(int code, const std::string& message) : code(code), message(message) {}
        ~HttpException() {}

        const char* what() const throw()
        {
            return message.c_str();
        }
        int getCode() const { return (code);}
};