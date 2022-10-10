#include "Worker.hpp"

Worker::BadRequestException::BadRequestException(const std::string str) {
	message_ = str;
}

Worker::BadRequestException::~BadRequestException() throw() {}

const char *Worker::BadRequestException::what() const throw() {
	return message_.c_str();
}

Worker::ForbiddenException::ForbiddenException(const std::string str) {
	message_ = str;
}

Worker::ForbiddenException::~ForbiddenException() throw() {}

const char *Worker::ForbiddenException::what() const throw() {
	return message_.c_str();
}

Worker::FileNotFoundException::FileNotFoundException(const std::string str) {
	message_ = str;
}

Worker::FileNotFoundException::~FileNotFoundException() throw() {}

const char *Worker::FileNotFoundException::what() const throw() {
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

Worker::Worker(int listenSocket) {
	struct sockaddr_in	clientAddress;
	socklen_t			addressLen = sizeof(clientAddress);

	connectSocket_ = accept(listenSocket, (struct sockaddr *)&clientAddress, &addressLen);
	if (connectSocket_ == -1) {
		close(listenSocket);
		throw std::runtime_error("fail: accept()\n");
	}
}

Worker::~Worker() {
	resetPollfd();
	delete request_;
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
			if (request_->getMethod() == "GET") {
				std::string filePath = std::string(WEB_ROOT) + request_->getUri();
				if (isDirectory(filePath))
					filePath += "/index.html"; // TODO: read default file from config
				if (isCgi(filePath)) {
					Cgi cgi(request_);
					std::string result = cgi.execute();
					Response response(HTTP_OK, result);
					send(response.createMessage().c_str());
					return ret;
				}
				if (isFileExist(filePath) == FT_FALSE)
					throw FileNotFoundException("File not found");
				Response response(HTTP_OK, fileToString(filePath));
				send(response.createMessage().c_str());
			}
			if (request_->getMethod() == "POST") {
				std::string filePath = std::string(WEB_ROOT) + request_->getUri();
				// TODO: validate filePath
				if (isCgi(filePath)) {
					Cgi cgi(request_);
					std::string result = cgi.execute();
					Response response(HTTP_CREATED, result);
					send(response.createMessage().c_str());
					return ret;
				}
			}
			return ret;
		}
		return ret;
	} catch(BadRequestException &e) {
		std::cerr << e.what() << std::endl;
		Response response(HTTP_BAD_REQUEST, fileToString(std::string(ERROR_PAGES_PATH) + "400.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
	} catch(ForbiddenException &e) {
		std::cerr << e.what() << std::endl;
		Response response(HTTP_FORBIDDEN, fileToString(std::string(ERROR_PAGES_PATH) + "403.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
	} catch(FileNotFoundException &e) {
		std::cerr << e.what() << std::endl;
		Response response(HTTP_NOT_FOUND, fileToString(std::string(ERROR_PAGES_PATH) + "404.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
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
		Response response(HTTP_INTERNAL_SERVER_ERROR, fileToString(std::string(ERROR_PAGES_PATH) + "500.html"));
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
		throw std::runtime_error("fail: recv()\n");
	buf_[ret] = '\0';
	std::cout << "server received(" << ret << "): " << buf_ << std::endl;
	request_ = new Request(buf_);
	validate();
	return FT_TRUE;
}

void Worker::validate() {
	std::vector<std::string> allowedMethods;

	allowedMethods.push_back("GET");
	allowedMethods.push_back("POST");
	allowedMethods.push_back("DELETE");
	if (!isIncluded(request_->getMethod(), allowedMethods))
		throw InvalidMethodException("Invalid method");
	if (request_->getVersion() != "HTTP/1.1")
		throw InvalidVersionException("HTTP version is not 1.1");
	const std::vector<std::string> *headerValues = request_->getHeaderValues("host");
	if (headerValues->size() != 1)
		throw BadRequestException("Invalid Host header");
	delete headerValues;
}

void Worker::send(const char *str) {
	int	ret = FT_TRUE;

	ret = ::send(pollfd_->fd, str, std::strlen(str), 0);
	if (ret == FT_ERROR)
		throw std::runtime_error("fail: send()\n");
}

void Worker::resetPollfd() {
	close(pollfd_->fd);
	pollfd_->fd = -1;
	pollfd_->revents = 0;
}
