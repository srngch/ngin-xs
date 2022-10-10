#include "Master.hpp"

Master::MasterException::MasterException(const std::string str) {
	message_ = str;
}

Master::MasterException::~MasterException() throw() {}

const char *Master::MasterException::what() const throw() {
	return message_.c_str();
}

Master::Master(char **env) {
	struct sockaddr_in	serverAddress;

	init(serverAddress);
	bind(serverAddress);
	listen();
	setEnv(env);
}

Master::~Master() {
	std::vector<Worker *>::iterator itBegin = workers_.begin();
	std::vector<Worker *>::iterator itEnd = workers_.end();

	while (itBegin != itEnd) {
		delete *itBegin;
		++itBegin;
	}
}

void Master::setEnv(char **originalEnv) {
	while (originalEnv && *originalEnv) {
		env_.push_back(std::string(*originalEnv));
		originalEnv++;
	}
}

void Master::addEnv(std::string new_env) {
	env_.push_back(new_env);
}

void	Master::init(struct sockaddr_in &serverAddress) {
	int yes = 1;

	listenSocket_ = socket(PF_INET, SOCK_STREAM, 0);
	std::cout << "listenSocket: " << listenSocket_ << std::endl;
	if (listenSocket_ == FT_ERROR)
		throw MasterException("fail: socket()\n");
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_len = sizeof(struct sockaddr_in);
	serverAddress.sin_family = PF_INET;
	serverAddress.sin_port = htons(4242);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
}

void	Master::bind(struct sockaddr_in &serverAddress) {
	int	ret;

	ret = ::bind(listenSocket_, (const struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (ret == FT_ERROR) {
		close(listenSocket_);
		throw MasterException("fail: bind()\n");
	}
}

void	Master::listen() {
	int ret;

	ret = ::listen(listenSocket_, 2);
	if (ret == -1) {
		close(listenSocket_);
		throw MasterException("fail: listen()\n");
	}
}

void	Master::run() {
	int	i;
	int	ret;

	pollfds_[0].fd = listenSocket_;
	pollfds_[0].events = POLLIN | POLLOUT;
	pollfds_[0].revents = 0;

	for (i = 1; i < POLLFDSLEN; i++)
		pollfds_[i].fd = -1;

	while (1) {
		ret = poll(pollfds_, POLLFDSLEN, -1);
		if (ret == -1) {
			close(listenSocket_);
			throw MasterException("fail: poll()\n");
		}
		if (pollfds_[0].revents & POLLIN) {
			Worker	*worker = new Worker(listenSocket_);
			worker->setPollfd(findEmptyPollfd());
			workers_.push_back(worker);
		}

		std::vector<Worker *>::iterator itBegin = workers_.begin();
		std::vector<Worker *>::iterator itEnd = workers_.end();

		while (itBegin != itEnd) {
			ret = (*itBegin)->work();
			if (ret == FT_FALSE) {
				delete *itBegin;
				itBegin = workers_.erase(itBegin);
				continue ;
			}
			++itBegin;
		}
	}
	close(listenSocket_);
}

struct pollfd	*Master::findEmptyPollfd()
{
	for (int i = 1; i < POLLFDSLEN; i++) {
		if (pollfds_[i].fd == -1)
			return &pollfds_[i];
	}
	return nullptr;
}
