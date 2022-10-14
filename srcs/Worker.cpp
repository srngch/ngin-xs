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

Worker::NotImplementedException::NotImplementedException(const std::string str) {
	message_ = str;
}

Worker::NotImplementedException::~NotImplementedException() throw() {}

const char *Worker::NotImplementedException::what() const throw() {
	return message_.c_str();
}

Worker::InvalidVersionException::InvalidVersionException(const std::string str) {
	message_ = str;
}

Worker::InvalidVersionException::~InvalidVersionException() throw() {}

const char *Worker::InvalidVersionException::what() const throw() {
	return message_.c_str();
}

Worker::Worker(int listenSocket)
	: isHeaderSet_(FT_FALSE), isRecvCompleted_(FT_FALSE) {
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
	ft_bool	ret = FT_TRUE;

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
		} else if (pollfd_->revents == POLLOUT && isRecvCompleted_ == FT_TRUE) {
			request_->setBody(originalBody_);
			validate();
			isRecvCompleted_ = FT_FALSE;
			// std::cout << "originalHeader_: " << originalHeader_ << std::endl;
			// std::cout << "originalBody_: " << originalBody_ << std::endl;
			// TODO: Config 체크해서 요청 URI가 허용하는 메소드인지 체크
			filePath_ = std::string(WEB_ROOT) + request_->getUri()->getOriginalUri();
			if (request_->getMethod() == "GET")
				return executeGet();
			if (request_->getMethod() == "POST")
				return executePost();
			if (request_->getMethod() == "DELETE")
				return executeDelete();
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
	} catch(NotImplementedException &e) {
		std::cerr << e.what() << std::endl;
		Response response(HTTP_NOT_IMPLEMENTED, fileToString(std::string(ERROR_PAGES_PATH) + "501.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
	} catch(InvalidVersionException &e) {
		std::cerr << e.what() << std::endl;
		Response response(HTTP_VERSION_NOT_SUPPORTED, fileToString(std::string(ERROR_PAGES_PATH) + "505.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
	} catch (std::exception &e) {
		std::cerr << "std::exception: " << e.what() << std::endl;
		Response response(HTTP_INTERNAL_SERVER_ERROR, fileToString(std::string(ERROR_PAGES_PATH) + "500.html"));
		send(response.createMessage().c_str());
		return FT_TRUE;
	}
}

ft_bool Worker::recv() {
	int 		ret;
	char		buf[BUFFER_LENGTH];
	std::size_t	index;
	std::string	transferEncoding;

	memset(buf, 0, BUFFER_LENGTH);
	ret = ::recv(pollfd_->fd, buf, BUFFER_LENGTH - 1, 0);
	buf[ret] = '\0';
	// std::cout << "server received(" << ret << "): " << buf << std::endl;
	if (isHeaderSet_ == FT_FALSE) {
		originalHeader_ += buf;
		index = originalHeader_.find(EMPTY_LINE);
		if (index != std::string::npos) {
			isHeaderSet_ = FT_TRUE;
			originalBody_ = originalHeader_.substr(index + strlen(EMPTY_LINE), std::string::npos);
			originalHeader_ = originalHeader_.substr(0, index);
			request_ = new Request(originalHeader_);
			if (
				ret < BUFFER_LENGTH && 
				originalBody_.length() <= std::size_t(atoi(request_->getHeaderValue("content-length").c_str()))
			)
				isRecvCompleted_ = FT_TRUE;
			transferEncoding = request_->getHeaderValue("transfer-encoding");
			if (
				transferEncoding == "chunked" &&
				originalBody_.find(EMPTY_LINE) != std::string::npos
			)
				isRecvCompleted_ = FT_TRUE;
		}
	} else if (isRecvCompleted_ == FT_FALSE) {
		originalBody_ += buf;
		transferEncoding = request_->getHeaderValue("transfer-encoding");
		if (transferEncoding == "chunked") {
			if (request_->getHeaderValue("content-length").length() > 0)
				throw BadRequestException("recv: Chunked message with content length");
			index = originalBody_.find(EMPTY_LINE);
			if (index != std::string::npos) {
				originalBody_ = originalBody_.substr(0, index);
				isRecvCompleted_ = FT_TRUE;
			}
		} else if (transferEncoding.length() > 0 && transferEncoding != "chunked") {
			throw NotImplementedException("Not Implemented");
		} else if (originalBody_.length() >= std::size_t(atoi(request_->getHeaderValue("content-length").c_str()))) {
			isRecvCompleted_ = FT_TRUE;
		}
	}
	if (ret == FT_FALSE) {
		resetPollfd();
		std::cout << "Client finished connection." << std::endl;
		return FT_FALSE;
	}
	if (ret == FT_ERROR)
		throw std::runtime_error("fail: recv()\n");
	return FT_TRUE;
}

void Worker::validate() {
	std::vector<std::string>	allowedMethods;

	allowedMethods.push_back("GET");
	allowedMethods.push_back("POST");
	allowedMethods.push_back("DELETE");
	if (!isIncluded(request_->getMethod(), allowedMethods))
		throw InvalidMethodException("Invalid method");
	if (request_->getVersion() != "HTTP/1.1")
		throw InvalidVersionException("HTTP version is not 1.1");
	if (request_->getHeaderValue("host").length() == 0)
		throw BadRequestException("Host header does not exist"); 
}

ft_bool Worker::executeGet() {
	if (isDirectory(filePath_))
		filePath_ += "/index.html"; // TODO: read default file from config
	if (isCgi(filePath_)) {
		Cgi cgi(request_);
		std::string result = cgi.execute();
		Response response(HTTP_OK, result);
		send(response.createMessage().c_str());
		return FT_TRUE;
	}
	if (isFileExist(filePath_) == FT_FALSE)
		throw FileNotFoundException("File not found");
	Response response(HTTP_OK, fileToString(filePath_));
	send(response.createMessage().c_str());
	return FT_TRUE;
}

ft_bool Worker::executePost() {
	// TODO: validate filePath
	if (isCgi(filePath_)) {
		Cgi cgi(request_);
		std::string result = cgi.execute();
		Response response(HTTP_CREATED, result);
		send(response.createMessage().c_str());
	}
	return FT_TRUE;
}

ft_bool Worker::executeDelete() {
	if (isFileExist(filePath_) == FT_FALSE)
		throw FileNotFoundException("File not found");
	if (isDirectory(filePath_) == FT_TRUE)
		throw ForbiddenException("Forbidden");
	if (unlink(filePath_.c_str()))
		throw std::runtime_error("Delete failed");
	Response response(HTTP_NO_CONTENT, "");
	send(response.createMessage().c_str());
	return FT_TRUE;
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
