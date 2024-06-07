#pragma once

#include "main.hpp"

class Response
{
    public:
        Response();
        Response(int code, const std::string& message);
        Response(const Response& other);
        Response& operator=(const Response& other);
        ~Response();
        void set_status(int code, const std::string& message);

        void add_header(const std::string& name, const std::string& value);

        void set_body(const std::string& b);

        std::string to_string() const;
    private:
        std::map<std::string, std::string> headers;
        std::string body;
    public:
        int status_code;
        std::string status_message;
};
