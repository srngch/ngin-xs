#ifndef __CGI_HPP__
#define __CGI_HPP__

#include "types.hpp"
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
	Request			*request_;
	vectorString	env_;
	vectorChar		result_;

	void	setEnv();
	char	**getEnv();

	Cgi();

public:
	Cgi(Request *request);
	~Cgi();

	const vectorChar	&execute();
};

#endif
