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
#include <fstream>
#include <cstdio>
#include "macro.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "autoindex/Autoindex.hpp"

#define BUFFER_LENGTH 50000

class Worker {
private:
	std::vector<std::string>	env_;
	int							connectSocket_;
	char						buf_[BUFFER_LENGTH];
	struct pollfd				*pollfd_;
	Request						*request_;

	ft_bool	recv();
	// void	unchunk();
	void	send(const char *str);
	void	resetPollfd();
	void	validate();
	std::string	executeCgiProgram(const std::string &filePath);
	char	**makeCgiEnv();

public:
	Worker(std::vector<std::string> env, int listenSocket);
	~Worker();
	
	char	**getEnv();
	void	setPollfd(struct pollfd *pollfd);
	ft_bool	work();

	class BadRequestException : public std::exception {
	private:
		std::string message_;
	public:
		BadRequestException(const std::string str);
		~BadRequestException() throw();
		virtual const char *what() const throw();
	};

	class ForbiddenException : public std::exception {
	private:
		std::string message_;
	public:
		ForbiddenException(const std::string str);
		~ForbiddenException() throw();
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
};

#endif
