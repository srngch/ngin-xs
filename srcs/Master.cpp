#include "Master.hpp"

Master::Master(const Block &serverBlock)
	: serverBlock_(serverBlock) {
	initListenSocket();
	bindListenSocket();
	listen();
}

Master::~Master() {
	std::vector<Worker *>::iterator	it;

	for (it = workers_.begin(); it != workers_.end(); it++)
		delete *it;
	close(listenSocket_);
}

void Master::initListenSocket() {
	int	yes = 1;

	listenSocket_ = socket(PF_INET, SOCK_STREAM, 0);
	std::cout << "listenSocket: " << listenSocket_ << std::endl;
	std::cout << "Port: " << serverBlock_.getPort() << std::endl;
	if (listenSocket_ == FT_ERROR)
		throw std::runtime_error("init: socket() failed");

	
	memset(&serverAddress_, 0, sizeof(serverAddress_));
	serverAddress_.sin_len = sizeof(struct sockaddr_in);
	serverAddress_.sin_family = PF_INET;
	serverAddress_.sin_port = htons(serverBlock_.getPort());
	serverAddress_.sin_addr.s_addr = htonl(inet_addr(serverBlock_.getHost().c_str()));
	setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
}

void Master::bindListenSocket() {
	int	ret;

	ret = ::bind(listenSocket_, (const struct sockaddr *)&serverAddress_, sizeof(serverAddress_));
	if (ret == FT_ERROR) {
		close(listenSocket_);
		throw std::runtime_error("bind: bind() failed");
	}
}

void Master::listen() {
	int	ret;

	ret = ::listen(listenSocket_, 100000);
	if (ret == -1) {
		close(listenSocket_);
		throw std::runtime_error("listen: listen() failed");
	}
}

void Master::run() {
	int								ret;
	std::vector<Worker *>::iterator	it = workers_.begin();

	while (it != workers_.end()) {
		ret = (*it)->work();
		if (ret == FT_FALSE) {
			delete *it;
			it = workers_.erase(it);
			continue ;
		}
		it++;
	}
}

void Master::appendWorker(struct pollfd *pollFd) {
	Worker	*worker = new Worker(listenSocket_, serverBlock_);

	worker->setPollfd(pollFd);
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
