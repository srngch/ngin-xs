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
#include "mime.hpp"

class Response {
private:
	int										statusCode_;
	std::string								statusLine_;
	std::map<std::string, std::string>		headers_;
	std::vector<char>						body_;
	std::vector<char>						message_;

	std::string makeDateValue();
	void makeDefaultHeaders();

	const std::string getHeaderValue(const std::string &fieldName);

public:
	Response(const std::string &status);
	Response(const std::string &status, const std::vector<char> &result, ft_bool isCgi = false);
	~Response();

	const std::vector<char>	&createMessage();

	void	setContentType(const std::string &fileExtension);
	void	appendHeader(const std::string &fieldName, const std::string &value);
};

#endif
