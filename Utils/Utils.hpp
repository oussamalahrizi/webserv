#pragma once

#include <iostream>
#include <vector>

class Utils
{
	private:
		Utils();
		~Utils();
	public:
		static std::vector<std::string> Split(const std::string& input, const std::string& delimiter);
		static std::vector<std::string> SplitByEach(const std::string& input, const std::string& delimiter);
		static std::string				Trim(const std::string& input);
};
