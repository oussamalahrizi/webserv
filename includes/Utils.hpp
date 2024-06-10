#pragma once

#include "common.hpp"

class ServerConf;

class Utils
{
	private:
		Utils();
		~Utils();
	public:
		static std::vector<std::string> Split(const std::string& input, const std::string& delimiter);
		static std::vector<std::string> SplitByEach(const std::string& input, const std::string& delimiter);
		static std::string				Trim(const std::string& input);
		static int						CheckNumeric(const std::string& value, size_t len);
		static int						findServer(const std::map<std::string, std::string>& hosts,
										ServerConf& s);
};


#include "ServerConf.hpp"
