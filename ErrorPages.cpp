
#include "includes/common.hpp"
#include "includes/HttpHandler.hpp"

std::string get_error_page(int status_code)
{
	return "<html>"
	"<head><title> " + http_codes[status_code] +  "</title></head>" 
	"<body>" 
	"<center><h1> " + http_codes[status_code] + " </h1></center>";
}
