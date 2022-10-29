#ifndef __MASTER_HPP__
#define __MASTER_HPP__

#include "types.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <exception>
#include "config/Block.hpp"
#include "Worker.hpp"
#include "macro.hpp"

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

	Master();

public:
	Master(const Block &serverBlock);
	~Master();
	
	void	run();
	void	appendWorker(struct pollfd *pollFd);

	const struct sockaddr_in	getServerAddress() const;
	int							getListenSocket() const;
	std::size_t					getPollIndex() const;
	
	void	setPollIndex(std::size_t pollIndex);

	class MasterException : public std::exception {
	private:
		std::string message_;
	public:
		MasterException(const std::string str);
		~MasterException() throw();
		const char *what() const throw();
	};
};

#endif
