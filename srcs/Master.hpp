#ifndef __MASTER_HPP__
#define __MASTER_HPP__

#include "Worker.hpp"

class Master {
private:
	vectorString			env_;
	std::string				host_;
	int						port_;
	struct sockaddr_in 		serverAddress_;
	int						listenSocket_;
	std::vector<Worker *>	workers_;
	std::vector<Block *>	serverBlocks_;
	std::size_t				pollIndex_;

	void	initListenSocket();
	void	bindListenSocket();
	void	listen();

public:
	Master(const std::string &hostPort, std::vector<Block *> serverBlocks);
	~Master();
	
	void	run();
	void	appendWorker(struct pollfd *pollFd);

	const struct sockaddr_in	getServerAddress() const;
	int							getListenSocket() const;
	std::size_t					getPollIndex() const;
	
	void	setPollIndex(std::size_t pollIndex);
};

#endif
