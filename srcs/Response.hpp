#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include <map>
#include <cctype>
#include <cstdlib>
#include <exception>
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
	void				appendHeader(const std::string &fieldName, const std::string &value);

	void	setContentType(const std::string &fileExtension);
};

#endif
