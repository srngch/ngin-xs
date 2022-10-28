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
#include <set>
#include "macro.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Cgi.hpp"
#include "autoindex/Autoindex.hpp"
#include "config/Block.hpp"

class Worker {
private:
	Block			serverBlock_;
	int				connectSocket_;
	struct pollfd	*pollfd_;
	Request			*request_;
	ft_bool			isHeaderSet_;
	ft_bool			isRecvCompleted_;
	ft_bool			isNewRequest_;

	ft_bool	recv();
	ft_bool	send(const std::vector<char> &message);
	void	resetPollfd();
	void	validate();
	ft_bool	executePutToTest(); // TODO: remove this
	ft_bool	executeGet();
	ft_bool	executePost();
	ft_bool	executeDelete();
	ft_bool	redirect(const std::string &des);
	void	initRequestState();
	ft_bool	isCgi(const std::string &filePath);

	Worker();

public:
	Worker(int listenSocket, const Block &serverBlock);
	~Worker();

	void	setPollfd(struct pollfd *pollfd);
	ft_bool	work();

	class HttpException : public std::exception {
	private:
		std::string message_;
		std::string	httpStatus_;
		int			httpCode_;

	public:
		HttpException(const std::string message, const std::string httpCode);
		~HttpException() throw();
		virtual const char *what() const throw();

		const std::string	&getHttpStatus();
		int					getHttpCode();
		std::vector<char>	makeErrorHtml(const std:: string &errorPage);
	};
};

#endif
