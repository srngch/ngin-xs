#ifndef __CGI_HPP__
#define __CGI_HPP__

#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <exception>
#include "macro.hpp"
#include "Request.hpp"
#include "utils.hpp"

class Cgi {
private:
	Request						*request_;
	std::vector<std::string>	env_;

	void setEnv();
	char	**getEnv();

public:
	Cgi(Request *request);
	~Cgi();

	std::string	execute();
};

#endif
