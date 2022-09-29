#include "Worker.hpp"

Worker::WorkerException::WorkerException(const std::string str) {
	message_ = str;
}

Worker::WorkerException::~WorkerException() throw() {}


const char *Worker::WorkerException::what() const throw() {
	return message_.c_str();
}

Worker::Worker(int listenSocket) {
	struct sockaddr_in	clientAddress;
	socklen_t			addressLen;

	addressLen = sizeof(clientAddress);
	connectSocket_ = accept(listenSocket, (struct sockaddr *)&clientAddress, &addressLen);
	if (connectSocket_ == -1) {
		close(listenSocket);
		throw WorkerException("fail: accept()\n");
	}
}

Worker::~Worker() {
	resetPollfd();
}

void	Worker::setPollfd(struct pollfd *pollfd) {
	pollfd_ = pollfd;
	pollfd_->fd = connectSocket_;
	pollfd_->events = POLLIN | POLLOUT;
	pollfd_->revents = 0;
}

bool Worker::work() {
	int		ret = FT_TRUE;

	if (pollfd_->revents == 0)
		return FT_TRUE;
	if (pollfd_->revents & POLLHUP) {
		resetPollfd();
		std::cout << "socket closed." << std::endl;
		return FT_FALSE;
	} else if (pollfd_->revents == pollfd_->events) {
		ret = recv();
		if (ret == FT_FALSE)
			return ret;
		send();
		return ret;
	}
	return ret;
}

bool Worker::recv() {
	int ret;
	
	ret = ::recv(pollfd_->fd, buf_, BUFFER_LENGTH, 0);
	if (ret == FT_FALSE) {
		resetPollfd();
		std::cout << "Client finished connection." << std::endl;
		return FT_FALSE;
	}
	if (ret == FT_ERROR)
		throw WorkerException("fail: recv()\n");
	buf_[ret] = '\0';
	std::cout << "server received(" << ret << "): " << buf_ << std::endl;
	return FT_TRUE;
}

void Worker::send() {
	int	ret = FT_TRUE;

	ret = ::send(pollfd_->fd, buf_, BUFFER_LENGTH, 0);
	if (ret == FT_ERROR)
		throw WorkerException("fail: send()\n");
}

void Worker::resetPollfd() {
	close(pollfd_->fd);
	pollfd_->fd = -1;
	pollfd_->revents = 0;
}
