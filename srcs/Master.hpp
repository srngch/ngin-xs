#ifndef __MASTER_HPP__
#define __MASTER_HPP__

#include "Worker.hpp"
class Master {
private:
	vectorString			env_;
	struct sockaddr_in 		serverAddress_;
	int						listenSocket_;
	std::vector<Worker *>	workers_;
	Block					serverBlock_;
	std::size_t				pollIndex_;

	void	init();
	void	bind();
	void	listen();

public:
	Master(const Block &serverBlock);
	~Master();
	
	void	run();
	void	appendWorker(struct pollfd *pollFd);

	const struct sockaddr_in	getServerAddress() const;
	int							getListenSocket() const;
	std::size_t					getPollIndex() const;
	
	void	setPollIndex(std::size_t pollIndex);
};

#endif
