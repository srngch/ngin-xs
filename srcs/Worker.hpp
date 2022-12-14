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
	std::vector<Block *>	serverBlocks_;
	int						connectSocket_;
	struct pollfd			*pollfd_;
	Request					*request_;
	ft_bool					isHeaderSet_;
	ft_bool					isRecvCompleted_;
	ft_bool					isNewRequest_;

	void		setNewRequest();
	const Block	&findServerBlock(const std::string &serverName);
	void		findLocationBlock(const Block &serverBlock);
	ft_bool		processHeader(const char *buf, int ret);
	void		processChunkedBody();
	void		processBodyBeforeHeaderSet(int ret);
	void		processBodyAfterHeaderSet(const char *buf, int ret);
	ft_bool		recv();
	void		validate();
	ft_bool		executePutToTest();
	ft_bool		executeGet();
	ft_bool		executePost();
	ft_bool		executeDelete();
	ft_bool		redirect(const std::string &des);
	void		initRequestState();
	ft_bool		isCgi(const std::string &filePath);
	ft_bool		send(const vectorChar &message);
	void		resetPollfd();

public:
	Worker(int listenSocket, const std::vector<Block *> &serverBlocks);
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
