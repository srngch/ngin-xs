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
#include <exception>
#include "Worker.hpp"
#include "macro.hpp"

#define POLLFDSLEN 100

class Master {
private:
	std::vector<std::string>	env_;
	int							listenSocket_;
	std::vector<Worker *>		workers_;
	struct pollfd				pollfds_[POLLFDSLEN];
	// Config					config_;

	void	init(struct sockaddr_in &serverAddress);
	void	bind(struct sockaddr_in &serverAddress);
	void	listen();
	struct pollfd *findEmptyPollfd();

public:
	Master(char **env);
	~Master();
	
	void	run();

	void	setEnv(char **originalEnv);
	void	addEnv(std::string new_env);
	
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
