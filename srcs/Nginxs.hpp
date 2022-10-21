#ifndef __NGINXS_HPP__
#define __NGINXS_HPP__

#include <vector>
#include <exception>
#include "Master.hpp"
#include "Request.hpp"
#include "config/Config.hpp"

/*
 * POLLFDSLEN 
 * : number of servers + number of clients
 * thus, number of max client
 * : POLLFDSLEN - number of servers
*/
 
#define POLLFDSLEN 100

class Nginxs {
private:
	std::vector<Master>	masters_;
	Config				config_;
	struct pollfd		pollfds_[POLLFDSLEN];

	struct pollfd	*findEmptyPollfd();

	Nginxs();

public:
	Nginxs(const char *confFilePath);
	~Nginxs();

	void	run();
};

#endif
