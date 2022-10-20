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
#include <string>
#include <vector>
#include "macro.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Cgi.hpp"
#include "autoindex/Autoindex.hpp"

#define BUFFER_LENGTH 1024

class Worker {
private:
	int							connectSocket_;
	struct pollfd				*pollfd_;
	Request						*request_;
	ft_bool						isHeaderSet_;
	ft_bool						isRecvCompleted_;
	std::size_t					bodyLength_;

	ft_bool	recv();
	ft_bool	send(const std::vector<char> &message);
	void	resetPollfd();
	void	validate();
	ft_bool	executeGet();
	ft_bool	executePost();
	ft_bool	executeDelete();
	ft_bool	redirect(const std::string &des);

	Worker();

public:
	Worker(int listenSocket);
	~Worker();

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

	class NotImplementedException : public std::exception {
	private:
		std::string message_;
	public:
		NotImplementedException(const std::string str);
		~NotImplementedException() throw();
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
