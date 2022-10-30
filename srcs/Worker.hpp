#ifndef __WORKER_HPP__
#define __WORKER_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include "config/Block.hpp"
#include "Cgi.hpp"
#include "autoindex/Autoindex.hpp"
#include "Response.hpp"

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
	void	validate();
	ft_bool	executePutToTest(); // TODO: remove this
	ft_bool	executeGet();
	ft_bool	executePost();
	ft_bool	executeDelete();
	ft_bool	redirect(const std::string &des);
	void	initRequestState();
	ft_bool	isCgi(const std::string &filePath);
	ft_bool	send(const vectorChar &message);
	void	resetPollfd();

public:
	Worker(int listenSocket, const Block &serverBlock);
	~Worker();

	ft_bool	work();

	void	setPollfd(struct pollfd *pollfd);

	class HttpException : public std::exception {
	private:
		std::string message_;
		std::string	httpStatus_;
		int			httpCode_;

	public:
		HttpException(const std::string &message, const std::string &httpCode);
		~HttpException() throw();
		virtual const char *what() const throw();

		vectorChar	makeErrorHtml(const std::string &errorPage);

		const std::string	&getHttpStatus();
		int					getHttpCode();

	};
};

#endif
