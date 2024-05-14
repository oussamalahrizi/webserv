#include "Utils.hpp"

Utils::Utils() {}
Utils::~Utils() {}

std::vector<std::string> Utils::Split(const std::string& input, const std::string& delimiter)
{
	std::vector<std::string> lines;
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;

	while ((pos_end = input.find(delimiter, pos_start)) != std::string::npos)
	{
        token = input.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        lines.push_back(token);
    }
    lines.push_back(input.substr(pos_start));
    return lines;
}