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

	void	parseHeader(const std::vector<std::string> &splitedMessage);
	void	parseChunkedBody(const std::string &originalBody);

	Request();

public:
	Request(const std::string &header);
	~Request();

	const std::string							&getMethod();
	const Uri									*getUri();
	const std::string							&getVersion();
	const std::map<std::string, std::string>	&getHeaders();
	const std::string							getHeaderValue(std::string fieldName);
	const std::string							&getBody();

	void	setBody(std::string &body);
	void	appendHeader(std::string fieldName, std::string value);
};

#endif
