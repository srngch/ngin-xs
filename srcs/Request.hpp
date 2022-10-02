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

class Request {
private:
	std::string								method_;
	std::string								uri_;
	std::string								version_;
	std::multimap<std::string, std::string>	headers_;
	std::string								body_;

	void	parse(const std::vector<std::string> &splitedMessage);

public:
	Request(const std::string &originalMessage);
	~Request();

	const std::string								&getMethod();
	const std::string								&getUri();
	const std::string								&getVersion();
	const std::multimap<std::string, std::string>	&getHeaders();
	const std::vector<std::string>					*getHeaderValues(std::string fieldName);
	const std::string								&getBody();
};

#endif
