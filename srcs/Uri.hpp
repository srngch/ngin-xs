#ifndef __URI_HPP__
#define __URI_HPP__

#include <string>
#include <set>
#include <iostream>
#include "macro.hpp"

class Uri {
private:
	std::string originalUri_;
	std::string	uriDir_;
	std::string	dir_;	// 실제 디렉토리 (redirect 설정이 있는 경우 uriDir와 다른 값)
	std::string	basename_;
	std::string	pathInfo_;
	std::string	queryString_;

	Uri();

public:
	Uri(const std::string &originalUri, const std::set<std::string> &supportedExtensions);
	~Uri();

	const std::string	&getOriginalUri() const;
	const std::string	&getDir() const;
	const std::string	&getBaseName() const;
	const std::string	&getPathInfo() const;
	const std::string	&getQueryString() const;
	std::string			getParsedUri() const;
};

#endif
