#ifndef __URI_HPP__
#define __URI_HPP__

#include <string>
// #include <unistd.h>
// #include <fcntl.h>
// #include <iostream>
// #include <cctype>
#include <iostream>
#include "macro.hpp"

class Uri {
private:
	std::string originalUri_;
	std::string	uriDir_;
	std::string	dir_;
	std::string	basename_;
	std::string	pathInfo_;
	std::string	queryString_;

	Uri();

public:
	Uri(const std::string &originalUri);
	~Uri();

	const std::string &getOriginalUri() const;
	const std::string &getDir() const;
	const std::string &getBaseName() const;
	const std::string &getPathInfo() const;
	const std::string &getQueryString() const;
	
	// std::string getPath();
};

#endif
