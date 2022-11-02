#include "Master.hpp"

Master::Master(const std::string &hostPort, std::vector<Block *> serverBlocks)
	: serverBlocks_(serverBlocks) {
	vectorString s = split(hostPort, ":");
	
	host_ = s[0];
	port_ = atoi(s[1].c_str());
	initListenSocket();
	bindListenSocket();
	listen();
}

Master::~Master() {
	std::vector<Worker *>::iterator	itWorker;
	std::vector<Block *>::iterator itBlock;

	for (itWorker = workers_.begin(); itWorker != workers_.end(); itWorker++)
		delete *itWorker;
	close(listenSocket_);
}

void Master::initListenSocket() {
	int	yes = 1;

	listenSocket_ = socket(PF_INET, SOCK_STREAM, 0);
	std::cout << "listenSocket: " << listenSocket_ << std::endl;
	std::cout << "Port: " << port_ << std::endl;
	if (listenSocket_ == FT_ERROR)
		throw std::runtime_error("init: socket() failed");

	
	memset(&serverAddress_, 0, sizeof(serverAddress_));
	serverAddress_.sin_len = sizeof(struct sockaddr_in);
	serverAddress_.sin_family = PF_INET;
	serverAddress_.sin_port = htons(port_);
	serverAddress_.sin_addr.s_addr = htonl(inet_addr(host_.c_str()));
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
	Worker	*worker = new Worker(listenSocket_, serverBlocks_);

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
