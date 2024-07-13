#include "common.hpp"
#include "HttpHandler.hpp"

class ErrorPage
{
	private:
		char buffer[READ_SIZE];
		std::string headers;
		std::string filename;
		int fd;
	public:
		ErrorPage(data& payload);
		~ErrorPage();
};

std::string get_error_page(int status_code);