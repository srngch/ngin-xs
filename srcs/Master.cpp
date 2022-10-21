#include "Master.hpp"

Master::MasterException::MasterException(const std::string str) {
	message_ = str;
}

Master::MasterException::~MasterException() throw() {}

const char *Master::MasterException::what() const throw() {
	return message_.c_str();
}

Master::Master() {}

Master::Master(const Block &serverBlock)
	: serverBlock_(serverBlock) {
	init();
	bind();
	listen();
}

Master::~Master() {}

void	Master::init() {
	int yes = 1;

	listenSocket_ = socket(PF_INET, SOCK_STREAM, 0);
	std::cout << "listenSocket: " << listenSocket_ << std::endl;
	std::cout << "Port: " << serverBlock_.getPort() << std::endl;
	if (listenSocket_ == FT_ERROR)
		throw MasterException("init: socket() failed");
	memset(&serverAddress_, 0, sizeof(serverAddress_));
	serverAddress_.sin_len = sizeof(struct sockaddr_in);
	serverAddress_.sin_family = PF_INET;
	serverAddress_.sin_port = htons(serverBlock_.getPort());
	serverAddress_.sin_addr.s_addr = htonl(inet_addr(serverBlock_.getHost().c_str()));
	setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
}

void	Master::bind() {
	int	ret;

	ret = ::bind(listenSocket_, (const struct sockaddr *)&serverAddress_, sizeof(serverAddress_));
	if (ret == FT_ERROR) {
		close(listenSocket_);
		throw MasterException("bind: bind() failed");
	}
}

void	Master::listen() {
	int ret;

	ret = ::listen(listenSocket_, 2);
	if (ret == -1) {
		close(listenSocket_);
		throw MasterException("listen: listen() failed");
	}
}

void	Master::run() {
	int								ret;
	std::vector<Worker>::iterator	itBegin = workers_.begin();
	std::vector<Worker>::iterator	itEnd = workers_.end();

	while (itBegin != itEnd) {
		ret = itBegin->work();
		if (ret == FT_FALSE) {
			itBegin = workers_.erase(itBegin);
			continue ;
		}
		++itBegin;
	}
}

void	Master::appendWorker(struct pollfd *pollFd) {
	Worker	worker(listenSocket_);

	worker.setPollfd(pollFd);
	workers_.push_back(worker);
}

const struct sockaddr_in Master::getServerAddress() const {
	return serverAddress_;
}

int Master::getListenSocket() const {
	return listenSocket_;
}

std::size_t Master::getPollIndex() const {
	return pollIndex_;
}

void Master::setPollIndex(std::size_t pollIndex) {
	pollIndex_ = pollIndex;
}
