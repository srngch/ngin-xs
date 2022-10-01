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
	std::multimap<std::string, std::string>	headers_;
	std::string								body_;

	void makeDefaultHeaders();

public:
	Response(std::string status, std::string body);
	~Response();
	std::string	createMessage();
};

#endif
