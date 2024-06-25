#include "../includes/ChunkedBody.hpp"

ChunkedBody::ChunkedBody(data& payload)
{
    state = SIZE;
    total = 0;
    body_fd = payload.temp_fd;
    max_size = payload.handler.max_body_size;
}

int ChunkedBody::transfer(const std::string& buffer)
{
    this->body.append(buffer);

    int remaining;
    std::stringstream ss;
    size_t pos;
    while (!body.empty())
    {
        switch(state)
        {
            case SIZE:
                pos = body.find(CRLF);
                if (pos == std::string::npos)
                    return(0);
                sub = body.substr(0, pos);
                ss.clear();
                ss.str("");
                ss << std::hex << sub;
                ss >> chunk_size;
                if (ss.fail())
                {
                    std::cerr << "error reading chunk size" << std::endl;
                    throw HttpException(500);
                }
                if (chunk_size == 0)
                {
                    state = END;
                    break;
                }
                state = DATA;
                body = body.erase(0, pos + 2);
                break;
            case DATA:
                if (chunk_size != 0)
                {
                    remaining = ((size_t) chunk_size) > body.length() ? body.length() : chunk_size;
                    chunk_size -= remaining;
                    std::string sub = body.substr(0, remaining);
                    if (write(this->body_fd, sub.c_str(), sub.length()) < 0)
                        throw HttpException(500);
                    body = body.erase(0, remaining);
                    total += remaining;
                    if (total > max_size)
                    {
                        std::cout << "max size exceeded" << std::endl;
                        this->body.clear();
                        throw HttpException(413);
                    }
                }
                if (body.length() < 2)
                    return (0);
                if (chunk_size == 0)
                {
                    size_t pos = body.find(CRLF);
                    if (pos == std::string::npos)
                        return (0);
                    body = body.erase(0, pos + 2);
                    state = SIZE;
                    break;
                }
                break;
            case END:
                std::cout << "closing" << std::endl;
                close(this->body_fd);
                return (1);
        }
    }
    return (0);
}

ChunkedBody::~ChunkedBody() {}