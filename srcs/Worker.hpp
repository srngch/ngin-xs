#ifndef __WORKER_HPP__
#define __WORKER_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <exception>
#include <iostream>
#include "macro.hpp"

#define BUFFER_LENGTH 5

class Worker {
private:
	int				connectSocket_;
	char			buf_[BUFFER_LENGTH];
	struct pollfd	*pollfd_;

	// Request			*request;
	// Response			*response;

	bool	recv();
	void	send();
	void	resetPollfd();

public:
	Worker(int listenSocket);
	~Worker();
	
	void	setPollfd(struct pollfd *pollfd);
	bool	work();
	
	class WorkerException : public std::exception {
	private:
		std::string message_;
	public:
		WorkerException(const std::string str);
		~WorkerException() throw();
		virtual const char *what() const throw();
	};
};

#endif
