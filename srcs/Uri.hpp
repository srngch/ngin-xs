#ifndef __URI_HPP__
#define __URI_HPP__

#include <string>
#include <set>
#include <iostream>
#include "config/Block.hpp"
#include "macro.hpp"

class Uri {
private:
	std::string originalUri_;
	std::string	uriDir_;
	std::string	pathDir_;
	std::string	basename_;
	std::string	pathInfo_;
	std::string	queryString_;

	void	parseQueryString();
	void	parsePathInfo(const std::set<std::string> &supportedExtensions);
	void	parseBasename(const std::string &locationUri, const std::string &webroot);

	Uri();

public:
	Uri(const std::string &originalUri);
	~Uri();

	const std::string	&getOriginalUri() const;
	const std::string	&getPathDir() const;
	const std::string	&getBaseName() const;
	const std::string	&getPathInfo() const;
	const std::string	&getQueryString() const;
	std::string			getFilePath() const;
	std::string			getParsedUri() const;
	
	void	parseUri(const Block &locationBlock);
};

#endif
