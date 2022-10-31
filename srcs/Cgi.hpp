#ifndef __CGI_HPP__
#define __CGI_HPP__

#include <unistd.h>
#include <fcntl.h>
#include "Request.hpp"

class Cgi {
private:
	Request			*request_;
	vectorString	env_;
	vectorChar		result_;

	char	**getEnv();

	void	setEnv();

public:
	Cgi(Request *request);
	~Cgi();

	const vectorChar	&execute();
};

#endif
