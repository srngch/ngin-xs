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
#include "Request.hpp"
#include "Response.hpp"

#define BUFFER_LENGTH 1024

class Worker {
private:
	int				connectSocket_;
	char			buf_[BUFFER_LENGTH];
	struct pollfd	*pollfd_;
	Request			*request_;

	ft_bool	recv();
	// void	unchunk();
	void	send(const char *str);
	void	resetPollfd();
	void	validate(Request &request);

public:
	Worker(int listenSocket);
	~Worker();
	
	void	setPollfd(struct pollfd *pollfd);
	ft_bool	work();
	
	class WorkerException : public std::exception {
	private:
		std::string message_;
	public:
		WorkerException(const std::string str);
		~WorkerException() throw();
		virtual const char *what() const throw();
	};

	class InvalidMethodException : public std::exception {
	private:
		std::string message_;
	public:
		InvalidMethodException(const std::string str);
		~InvalidMethodException() throw();
		virtual const char *what() const throw();
	};

	class InvalidVersionException : public std::exception {
	private:
		std::string message_;
	public:
		InvalidVersionException(const std::string str);
		~InvalidVersionException() throw();
		virtual const char *what() const throw();
	};

	class InvalidHostHeaderException : public std::exception {
	private:
		std::string message_;
	public:
		InvalidHostHeaderException(const std::string str);
		~InvalidHostHeaderException() throw();
		virtual const char *what() const throw();
	};

	class FileNotFoundException : public std::exception {
	private:
		std::string message_;
	public:
		FileNotFoundException(const std::string str);
		~FileNotFoundException() throw();
		virtual const char *what() const throw();
	};
};

#endif
