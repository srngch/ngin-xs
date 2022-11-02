#ifndef __NGINXS_HPP__
#define __NGINXS_HPP__

#include "Master.hpp"
#include "config/Config.hpp"

#define POLLFDSLEN 140

class Nginxs {
private:
	std::vector<Master *>	masters_;
	Config					config_;
	struct pollfd			pollfds_[POLLFDSLEN];

	void			initPollFds();
	struct pollfd	*findEmptyPollfd();

public:
	Nginxs(const char *confFilePath);
	~Nginxs();

	void	run();
};

#endif
