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
    size_t pos_start = 0;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (delimiter.find(c) != std::string::npos) {
            // Found a delimiter character, split the input
            if (i > pos_start)
                lines.push_back(input.substr(pos_start, i - pos_start));
            pos_start = i + 1;
        }
    }
    // Add the remaining part of the input
    if (pos_start < input.size()) {
        lines.push_back(input.substr(pos_start));
    }
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

int Utils::CheckNumeric(const std::string& value, size_t len)
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