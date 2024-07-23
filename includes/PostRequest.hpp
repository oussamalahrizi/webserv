#pragma once

#include "RequestHandle.hpp"
#include "HttpHandler.hpp"
#include "common.hpp"

class PostRequest : public RequestHandle
{
	public:
		PostRequest(data& payload, int& status_code);
		int nextChunk(std::string& chunk, int& code);
		~PostRequest();
};
