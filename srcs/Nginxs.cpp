#include "Nginxs.hpp"

Nginxs::Nginxs() {}

Nginxs::Nginxs(const char *confFilePath) {
	config_.parseConfigFile(confFilePath);
}

Nginxs::~Nginxs() {
	std::vector<Master *>::iterator	masterIt;
	
	for (masterIt = masters_.begin(); masterIt != masters_.end(); masterIt++)
		delete *masterIt;
}

void Nginxs::run() {
	std::size_t	i = 0; 
	int			ret;

	std::vector<Block>				serverBlocks = config_.getServerBlocks();
	std::vector<Block>::iterator	it;
	std::vector<Master *>::iterator	masterIt;
	
	for (it = serverBlocks.begin();	it != serverBlocks.end(); it++)
	{
		Master *m = new Master(*it);
		m->setPollIndex(i);
		std::cout << "setPollIndex: " << i << std::endl;
		masters_.push_back(m);

		pollfds_[i].fd = m->getListenSocket();
		pollfds_[i].events = POLLIN | POLLOUT;
		pollfds_[i].revents = 0;	
		i++;
	}

	// Initialize pollfds for workers
	for (int fds = i; fds < POLLFDSLEN; fds++)
		pollfds_[fds].fd = -1;

	while (1) {
		ret = poll(pollfds_, POLLFDSLEN, 1000);
		if (ret == 0)
			continue ;
		if (ret == -1) {
			for (int fds = 0; fds < POLLFDSLEN; fds++) {
				if (pollfds_[fds].fd != -1)
					close(pollfds_[fds].fd);
			}
			throw std::runtime_error("run: poll() failed");
		}
		for(masterIt = masters_.begin(); masterIt != masters_.end(); masterIt++) {
			if (pollfds_[(*masterIt)->getPollIndex()].revents & POLLIN) {
				struct pollfd	*emptyPollFd = findEmptyPollfd();
			// 	// if (emptyPollFd == nullptr)
			// 	// 	throw 503
				// std::cout << "master: " << masterIt->getPollIndex() << std::endl;
				(*masterIt)->appendWorker(emptyPollFd);
			}
			(*masterIt)->run();
		}
	}
	for (int fds = 0; fds < POLLFDSLEN; fds++) {
		if (pollfds_[fds].fd != -1)
			close(pollfds_[fds].fd);
	}
}

struct pollfd *Nginxs::findEmptyPollfd()
{
	for (int i = 0; i < POLLFDSLEN; i++) {
		if (pollfds_[i].fd == -1) {
			std::cout << "pollfd index: " << i << std::endl;
			return &pollfds_[i];
		}
	}
	return nullptr;
}
