#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <exception>
#include "macro.hpp"
#include "utils.hpp"

class Response {
private:
	int										statusCode_;
	std::string								statusLine_;
	std::map<std::string, std::string>		headers_;
	std::vector<char>						body_;

	void makeDefaultHeaders();

public:
	Response(std::string status);
	Response(std::string status, const std::vector<char> &body);
	~Response();

	std::vector<char>	createMessage();

	void	appendHeader(std::string fieldName, std::string value);
};

#endif
