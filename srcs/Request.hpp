#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <iostream>
#include "macro.hpp"

class Request {
private:
	std::string								method_;
	std::string								uri_;
	std::string								version_;
	std::multimap<std::string, std::string>	headers_;
	std::string								body_;

	void	parse(const std::vector<std::string> &splitedMessage);
	std::vector<std::string>				split(const std::string &str, std::string delim);

public:
	Request(const std::string &originalMessage);
	~Request();
};

#endif
