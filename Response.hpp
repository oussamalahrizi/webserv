#pragma once

#include <string>
#include <sstream>
#include <map>

class Response {
public:
    Response() : status_code(200), status_message("OK") {}

    void set_status(int code, const std::string& message) {
        status_code = code;
        status_message = message;
    }

    void add_header(const std::string& name, const std::string& value) {
        headers[name] = value;
    }

    void set_body(const std::string& b) {
        body = b;
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << status_code;
        std::string response = "HTTP/1.1 " + std::string(ss.str()) + " " + status_message + "\r\n";
        for(std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            response += it->first + ": " + it->second + "\r\n";
        }
        response += "\r\n" + body;
        return response;
    }

private:
    int status_code;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::string body;
};
