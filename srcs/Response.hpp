#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include "types.hpp"
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
	int					statusCode_;
	std::string			statusLine_;
	mapStringString		headers_;
	vectorChar			body_;
	vectorChar			message_;

	std::string	makeDateValue();
	void		makeDefaultHeaders();

	const std::string	getHeaderValue(const std::string &fieldName);

public:
	Response(const std::string &status);
	Response(const std::string &status, const vectorChar &result, ft_bool isCgi = false);
	~Response();

	const vectorChar	&createMessage();

	void	setContentType(const std::string &fileExtension);
	void	appendHeader(const std::string &fieldName, const std::string &value);
};

#endif
