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
	// ft_bool	validate();

public:
	Request(const std::string &originalMessage);
	~Request();
};

#endif
