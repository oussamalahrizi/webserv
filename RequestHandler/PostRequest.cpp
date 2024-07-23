#include "../includes/PostRequest.hpp"

PostRequest::PostRequest(data& payload, int& status_code)
{
	if (payload.loc.up)
		status_code = 201;
	else
		status_code = 202;
}

PostRequest::~PostRequest() {}

int PostRequest::nextChunk(std::string& chunk, int& code)
{
	chunk = "HTTP/1.1 " + http_codes[code] + " \r\n";
	chunk += "Connection: close";
	chunk += DCRLF;
	return (1);
}