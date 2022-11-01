#include "Nginxs.hpp"

Nginxs::Nginxs(const char *confFilePath) {
	config_.parseConfigFile(confFilePath);
}

Nginxs::~Nginxs() {
	std::vector<Master *>::iterator	it;
	
	for (it = masters_.begin(); it != masters_.end(); it++)
		delete *it;
}

void	Nginxs::initPollFds() {
	// std::size_t						i = 0; 
	// std::vector<Block>				serverBlocks = config_.getServerBlocks();
	// std::vector<Block>::iterator	it;

	// Test getPorts
	setString		hostPorts = config_.getHostPorts();
	setStringIter	testIt;
	
	std::cout << "<hostPorts>" << std::endl;
	for (testIt = hostPorts.begin(); testIt != hostPorts.end(); testIt++) {
		std::cout << "hostPorts: " << *testIt << std::endl;
	}

	// TODO: 포트 개수만큼 master 만들기
	// for (it = serverBlocks.begin();	it != serverBlocks.end(); it++) {
	// 	Master *m = new Master(*it);
	// 	m->setPollIndex(i);
	// 	masters_.push_back(m);

	// 	pollfds_[i].fd = m->getListenSocket();
	// 	pollfds_[i].events = POLLIN | POLLOUT;
	// 	pollfds_[i].revents = 0;	
	// 	i++;
	// }

	// // Initialize pollfds for workers
	// for (int fds = i; fds < POLLFDSLEN; fds++)
	// 	pollfds_[fds].fd = -1;
}

struct pollfd *Nginxs::findEmptyPollfd()
{
	for (int i = 0; i < POLLFDSLEN; i++) {
		if (pollfds_[i].fd == -1)
			return &pollfds_[i];
	}
	return nullptr;
}

void Nginxs::run() {
	int								ret;
	std::vector<Master *>::iterator	it;
	
	initPollFds();

	while (FT_TRUE) {
		ret = poll(pollfds_, POLLFDSLEN, 1000);
		if (ret == FT_FALSE)
			continue ;
		if (ret == FT_ERROR) {
			for (int fds = 0; fds < POLLFDSLEN; fds++) {
				if (pollfds_[fds].fd != -1)
					close(pollfds_[fds].fd);
			}
			throw std::runtime_error("run: poll() failed");
		}
		for (it = masters_.begin(); it != masters_.end(); it++) {
			if (pollfds_[(*it)->getPollIndex()].revents & POLLIN)
				(*it)->appendWorker(findEmptyPollfd());
			(*it)->run();
		}
	}

	// close opened poll fds
	for (int fds = 0; fds < POLLFDSLEN; fds++) {
		if (pollfds_[fds].fd != -1)
			close(pollfds_[fds].fd);
	}
}
