#pragma once

#include "common.hpp"

enum
{
	HEAD,
	TITLE,
	CONTENT,
	TAIL
};

class Autoindex
{
	private:
		DIR *dir;
		std::vector<std::string> fileList;
		int current_index;
		std::string dir_path;
		int headers;
		int state;
		int error;
		std::string href;
	private:
		void generateFileList();
		int getContentLength();
		std::string getHead() const;
		std::string getTitle() const;
		std::string getTail() const;
		std::string getFileInfo(const std::string& filename);
	public:
		Autoindex(const std::string& path, int& code, const std::string& root);
		int next_chunk(std::string& chunk, int& code);
		~Autoindex();
};

