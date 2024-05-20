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

std::vector<std::string> Utils::SplitByEach(const std::string& input, const std::string& delimiter)
{
    std::vector<std::string> lines;
    size_t pos_start = 0, pos_end;
    std::string token;

    while ((pos_end = input.find_first_of(delimiter, pos_start)) != std::string::npos)
    {
        token = input.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + 1;
        lines.push_back(token);
    }

    lines.push_back(input.substr(pos_start));
    return lines;
}

std::string Utils::Trim(const std::string& input)
{
    size_t start = 0;
    while (start < input.length() && std::isspace(input[start]))
        start++;
    size_t end = input.length();
    while (end > start && std::isspace(input[end - 1]))
        end--;
    return input.substr(start, end - start);
}

int Utils::CheckNumeric(const std::string& value, int len)
{
    if (value.length() != len || value.length() < len)
        return 0;
    size_t i = 0;
    while (i < len)
    {
        if (!std::isdigit(value[i]))
            return (0);
        i++;
    }
    return (1);
}