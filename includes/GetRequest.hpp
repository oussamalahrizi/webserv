#pragma once

#include "RequestHandle.hpp"
#include "HttpHandler.hpp"

class GetRequest : public RequestHandle
{
	private:
		int fd;
		data payload;
		std::string file;
		int error;
	private:
		void handleServeRoot();
		void setError(int code);
	public:
		GetRequest(data& payload);
		int nextChunk(std::string& chunk, int& status_code);
		~GetRequest();
};
