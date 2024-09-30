#include "../includes/LengthBody.hpp"


LengthBody::LengthBody() {}

LengthBody::LengthBody(data& payload)
{
    std::stringstream ss;
    std::string cl_string = payload.headers["Content-Length"];
    ss << cl_string;
    ss >> this->cl;
    if (ss.fail())
        throw HttpException(500);
    this->file_stream.open(payload.tempfile_name.c_str(), std::ios::out | std::ios::trunc);
    if (!file_stream.is_open())
        throw HttpException(500);
    this->max_size = payload.handler.max_body_size;
    total = 0;
}

LengthBody::LengthBody(const LengthBody& other)
{
    *this = other;
}

LengthBody& LengthBody::operator=(const LengthBody& other)
{
    (void) other;
    return *this;
}

LengthBody::~LengthBody() {}

int LengthBody::transfer(const std::string& buffer)
{
	size_t readed = buffer.length();
    size_t to_add = readed <= cl ? readed : cl;
    cl -= to_add;
    total += to_add;
    if (total > max_size)
        throw HttpException(413);
    file_stream << buffer;
    if (file_stream.fail())
        throw HttpException(500);
    if (!cl)
    {
        file_stream.close();
        std::cout << total << std::endl;
        return (1);
    }
    return (0);
}