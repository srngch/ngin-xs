#include "Worker.hpp"

Worker::WorkerException::WorkerException(const std::string str) {
	message_ = str;
}

Worker::WorkerException::~WorkerException() throw() {}


const char *Worker::WorkerException::what() const throw() {
	return message_.c_str();
}

Worker::InvalidMethodException::InvalidMethodException(const std::string str) {
	message_ = str;
}

Worker::InvalidMethodException::~InvalidMethodException() throw() {}

const char *Worker::InvalidMethodException::what() const throw() {
	return message_.c_str();
}

Worker::InvalidVersionException::InvalidVersionException(const std::string str) {
	message_ = str;
}

Worker::InvalidVersionException::~InvalidVersionException() throw() {}

const char *Worker::InvalidVersionException::what() const throw() {
	return message_.c_str();
}

Worker::InvalidHostHeaderException::InvalidHostHeaderException(const std::string str) {
	message_ = str;
}

Worker::InvalidHostHeaderException::~InvalidHostHeaderException() throw() {}

const char *Worker::InvalidHostHeaderException::what() const throw() {
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

ft_bool Worker::work() {
	int		ret = FT_TRUE;

	try {
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
			// 
			send(buf_);
			return ret;
		}
		return ret;
	} catch(InvalidMethodException &e) {
		std::cerr << e.what() << std::endl;
		Response response(HTTP_METHOD_NOT_ALLOWED, fileToString(std::string(ERROR_PAGES_PATH) + "405.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
	} catch(InvalidVersionException &e) {
		std::cerr << e.what() << std::endl;
		Response response(HTTP_VERSION_NOT_SUPPORTED, fileToString(std::string(ERROR_PAGES_PATH) + "505.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		Response response(HTTP_BAD_REQUEST, fileToString(std::string(ERROR_PAGES_PATH) + "400.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
	}
}

ft_bool Worker::recv() {
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
	Request request(buf_);
	validate(request);
	return FT_TRUE;
}

void Worker::validate(Request &request) {
	std::vector<std::string> allowedMethods;

	allowedMethods.push_back("GET");
	allowedMethods.push_back("POST");
	allowedMethods.push_back("DELETE");
	if (!isIncluded(request.getMethod(), allowedMethods))
		throw InvalidMethodException("Invalid method");
	if (request.getVersion() != "HTTP/1.1")
		throw InvalidVersionException("HTTP version is not 1.1");
	const std::vector<std::string> *headerValues = request.getHeaderValues("host");
	if (headerValues->size() != 1)
		throw InvalidHostHeaderException("Invalid Host header");
	delete headerValues;
}

void Worker::send(const char *str) {
	int	ret = FT_TRUE;

	ret = ::send(pollfd_->fd, str, std::strlen(str), 0);
	if (ret == FT_ERROR)
		throw WorkerException("fail: send()\n");
}

void Worker::resetPollfd() {
	close(pollfd_->fd);
	pollfd_->fd = -1;
	pollfd_->revents = 0;
}
