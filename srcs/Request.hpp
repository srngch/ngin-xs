#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <exception>
#include "macro.hpp"
#include "utils.hpp"
#include "Uri.hpp"

class Request {
private:
	std::string							method_;
	Uri									*uri_;
	std::string							version_;
	std::map<std::string, std::string>	headers_;
	std::string							body_;
	std::string							filePath_;
	std::string							originalHeader_;
	std::string							originalBody_;

	void	parseHeader(const std::vector<std::string> &splitedMessage);
	void	parseChunkedBody();

public:
	Request();
	~Request();

	const std::string							&getMethod();
	const Uri									*getUri();
	const std::string							&getVersion();
	const std::map<std::string, std::string>	&getHeaders();
	const std::string							getHeaderValue(std::string fieldName);
	const std::string							&getBody();
	const std::string							&getFilePath();
	const std::string							&getOriginalHeader();
	const std::string							&getOriginalBody();

	std::size_t	getContentLengthNumber();
	
	void	setBody();
	void	setHeaders();
	void	setFilePath(const std::string &webRoot);
	void	setOriginalHeader(const std::string &originalHeader);
	void	setOriginalBody(const std::string &originalBody);

	void	appendOriginalHeader(const std::string &buf);
	void	appendOriginalBody(const std::string &buf);
};

#endif
