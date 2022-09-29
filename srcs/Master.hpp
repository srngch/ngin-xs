#ifndef __MASTER_HPP__
#define __MASTER_HPP__

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
// #include <cstdbool> C++ 11
#include <exception>
#include "Worker.hpp"
#include "macro.hpp"

#define POLLFDSLEN 100

class Master {
private:
	int					listenSocket_;
	std::vector<Worker *>	workers_;
	// Config			config_;
	struct pollfd		pollfds_[POLLFDSLEN];

	void	init(struct sockaddr_in &serverAddress);
	void	bind(struct sockaddr_in &serverAddress);
	void	listen();
	struct pollfd *findEmptyPollfd();

public:
	Master();
	~Master();
	
	void	run();
	
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
