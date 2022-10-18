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
	std::vector<char>					body_;
	std::string							filePath_;
	std::vector<char>					originalHeader_;
	std::vector<char>					originalBody_;

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
	const std::vector<char>						&getBody();
	const std::string							&getFilePath();
	const std::vector<char>						&getOriginalHeader();
	const std::vector<char>						&getOriginalBody();

	std::size_t	getContentLengthNumber();
	
	void	setBody();
	void	setHeaders();
	void	setFilePath(const std::string &webRoot);
	void	setOriginalHeader(const std::vector<char> originalHeader);
	void	setOriginalBody(const std::vector<char> originalBody);

	void	appendOriginalHeader(const char *buf, int length);
	void	appendOriginalBody(const char *buf, int length);
};

#endif
