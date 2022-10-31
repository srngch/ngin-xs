#ifndef __URI_HPP__
#define __URI_HPP__

#include "config/Block.hpp"

class Uri {
private:
	std::string	originalUri_;
	std::string	uriDir_;
	std::string	pathDir_;
	std::string	basename_;
	std::string	pathInfo_;
	std::string	queryString_;

	void	parseQueryString();
	void	parsePathInfo(const setString &supportedExtensions);
	void	parseBasename(const std::string &locationUri, const std::string &webroot);

public:
	Uri(const std::string &originalUri);
	~Uri();

	void	parseUri(const Block &locationBlock);

	const std::string	&getOriginalUri() const;
	const std::string	&getPathDir() const;
	const std::string	&getBaseName() const;
	const std::string	&getPathInfo() const;
	const std::string	&getQueryString() const;
	std::string			getFilePath() const;
	std::string			getParsedUri() const;
};

#endif
