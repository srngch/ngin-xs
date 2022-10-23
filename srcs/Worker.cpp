#include "Worker.hpp"

Worker::HttpException::HttpException(const std::string message, const std::string httpCode)
	: message_(message), httpCode_(httpCode) {}

Worker::HttpException::~HttpException() throw() {}

const char *Worker::HttpException::what() const throw() {
	return message_.c_str();
}

std::string Worker::HttpException::getHttpCode() {
	return httpCode_;
}

std::string Worker::HttpException::makeErrorHtml() {
	return "<!DOCTYPE html>\n\
<html>\n\
<head>\n\
	<meta charset=\"UTF-8\">\n\
	<title>" + httpCode_ + "</title>\n\
</head>\n\
<body>\n\
	<h1>" + httpCode_ + "</h1>\n\
	<hr />\n\
	ngin-xs\n\
</body>\n\
</html>";
}

Worker::Worker() {}

Worker::Worker(int listenSocket)
	: request_(nullptr), isHeaderSet_(FT_FALSE), isRecvCompleted_(FT_FALSE), bodyLength_(0) {
	struct sockaddr_in	clientAddress;
	socklen_t			addressLen = sizeof(clientAddress);

	connectSocket_ = accept(listenSocket, (struct sockaddr *)&clientAddress, &addressLen);
	std::cout << "connectSocket_: " << connectSocket_ << std::endl;
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
	ft_bool	tmp_isRecvCompleted;

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
			request_->setBody();
			initRequestState();
			validate();
			// TODO: Config 체크해서 요청 URI가 허용하는 메소드인지 체크
			request_->setFilePath(std::string(WEB_ROOT));
			if (request_->getMethod() == "GET")
				return executeGet();
			if (request_->getMethod() == "POST")
				return executePost();
			if (request_->getMethod() == "DELETE")
				return executeDelete();
		}
		return ret;
	} catch (HttpException &e) {
		std::cerr << e.what() << std::endl;
		tmp_isRecvCompleted = isRecvCompleted_;
		initRequestState();
		Response response(e.getHttpCode(), stringToCharV(e.makeErrorHtml()));
		return send(response.createMessage()) && tmp_isRecvCompleted;
	} catch (std::exception &e) {
		HttpException ex = HttpException(e.what(), HTTP_INTERNAL_SERVER_ERROR);

		std::cerr << "std::exception: " << e.what() << std::endl;
		tmp_isRecvCompleted = isRecvCompleted_;
		initRequestState();
		Response response(ex.getHttpCode(), stringToCharV(ex.makeErrorHtml()));
		return send(response.createMessage()) && tmp_isRecvCompleted;
	}
}

ft_bool Worker::recv() {
	int 						ret;
	char						buf[BUFFER_LENGTH];
	std::string					transferEncoding;
	std::vector<char>			originalHeader;
	std::vector<char>			originalBody;
	const char 					*crlf = EMPTY_LINE;
	std::vector<char>::iterator it;

	memset(buf, 0, BUFFER_LENGTH);
	ret = ::recv(pollfd_->fd, buf, BUFFER_LENGTH - 1, 0);
	buf[ret] = '\0';
	std::string client_max_body_size = "1000000"; // TODO: read from Config class
	if (isHeaderSet_ == FT_FALSE) {
		delete request_;
		request_ = new Request();
		request_->appendOriginalHeader(buf, ret);
		originalHeader = request_->getOriginalHeader();
		it = std::search(originalHeader.begin(), originalHeader.end(), crlf, crlf + strlen(crlf));
		if (it != originalBody.end()) {
			isHeaderSet_ = FT_TRUE;
			request_->setOriginalBody(std::vector<char>(it + strlen(EMPTY_LINE), originalHeader.end()));
			request_->setOriginalHeader(std::vector<char>(originalHeader.begin(), it));
			bodyLength_ = ret - request_->getOriginalHeader().size() - strlen(EMPTY_LINE);
			request_->setHeaders();
			transferEncoding = request_->getHeaderValue("transfer-encoding");
			if (transferEncoding.length() > 0 && transferEncoding != "chunked")
				throw HttpException("Not Implemented", HTTP_NOT_IMPLEMENTED);
			if (transferEncoding == "chunked" && request_->getHeaderValue("content-length").length() > 0)
				throw HttpException("recv: Chunked message with content length", HTTP_BAD_REQUEST);
			originalBody = request_->getOriginalBody();
			if (request_->getContentLengthNumber() > std::size_t(atol(client_max_body_size.c_str()))
				|| originalBody.size() > std::size_t(atol(client_max_body_size.c_str())))
				throw HttpException("recv: Content-length is larger than client max body size", HTTP_PAYLOAD_TOO_LARGE);
			if (ret < BUFFER_LENGTH && bodyLength_ >= request_->getContentLengthNumber())
				isRecvCompleted_ = FT_TRUE;
			it = std::search(originalBody.begin(), originalBody.end(), crlf, crlf + strlen(crlf));
			if (transferEncoding == "chunked" && it == originalBody.end())
				isRecvCompleted_ = FT_TRUE;
		}
	} else if (isRecvCompleted_ == FT_FALSE) {
		request_->appendOriginalBody(buf, ret);
		bodyLength_ += ret;
		originalBody = request_->getOriginalBody();
		transferEncoding = request_->getHeaderValue("transfer-encoding");
		if (transferEncoding == "chunked") {
			if (bodyLength_ > std::size_t(atol(client_max_body_size.c_str())))
				throw HttpException("recv: Body length is larger than client max body size", HTTP_PAYLOAD_TOO_LARGE);
			originalBody = request_->getOriginalBody();
			it = std::search(originalBody.begin(), originalBody.end(), crlf, crlf + strlen(crlf));
			if (it != originalBody.end()) {
				request_->setOriginalBody(std::vector<char>(originalBody.begin(), it));
				isRecvCompleted_ = FT_TRUE;
			}
		} else if (bodyLength_ >= request_->getContentLengthNumber()) {
			isRecvCompleted_ = FT_TRUE;
		}
	}
	if (ret == FT_FALSE) {
		resetPollfd();
		std::cout << "Client finished connection." << std::endl;
		return FT_FALSE;
	}
	if (ret == FT_ERROR)
		throw HttpException("recv: fail", HTTP_INTERNAL_SERVER_ERROR);
	return FT_TRUE;
}

void Worker::validate() {
	std::vector<std::string>	allowedMethods;

	allowedMethods.push_back("GET");
	allowedMethods.push_back("POST");
	allowedMethods.push_back("DELETE");
	if (!isIncluded(request_->getMethod(), allowedMethods))
		throw HttpException("validate: Invalid method", HTTP_BAD_REQUEST);
	if (request_->getVersion() != "HTTP/1.1")
		throw HttpException("validate: HTTP version is not 1.1", HTTP_VERSION_NOT_SUPPORTED);
	if (request_->getHeaderValue("host").length() == 0)
		throw HttpException("validate: Host header does not exist", HTTP_BAD_REQUEST);
}

ft_bool Worker::executeGet() {
	std::string	filePath;
	std::size_t	index;
	std::string	extension;

	filePath = request_->getFilePath();
	if (isDirectory(filePath) && request_->getUri()->getBaseName().length() > 0) {
		redirect(request_->getUri()->getOriginalUri() + "/");
		return FT_TRUE;
	}
	if (isDirectory(filePath))
		filePath += "index.html"; // TODO: read default file from config
	if (isCgi(filePath)) {
		Cgi cgi(request_);
		std::string result = cgi.execute();

		Response response(HTTP_OK, stringToCharV(result), FT_TRUE);
		return send(response.createMessage());
	}
	if (isFileExist(filePath) == FT_FALSE)
		throw HttpException("executeGet: File not found", HTTP_NOT_FOUND);

	Response response(HTTP_OK, fileToCharV(filePath));
	index = filePath.find_last_of(".");
	extension = filePath.substr(index + 1, std::string::npos);
	response.setContentType(extension);
	return send(response.createMessage());
}

ft_bool Worker::executePost() {
	// TODO: validate filePath
	if (isCgi(request_->getFilePath())) {
		Cgi cgi(request_);
		std::string result = cgi.execute();
		Response response(HTTP_CREATED, stringToCharV(result), FT_TRUE);
		return send(response.createMessage());
	}
	return FT_TRUE;
}

ft_bool Worker::executeDelete() {
	std::string	filePath;

	filePath = request_->getFilePath();
	if (isFileExist(filePath) == FT_FALSE)
		throw HttpException("executeDelete: File not found", HTTP_NOT_FOUND);
	if (isDirectory(filePath) == FT_TRUE)
		throw HttpException("executeDelete: Forbidden", HTTP_FORBIDDEN);
	if (unlink(filePath.c_str()))
		throw HttpException("executeDelete: Delete failed", HTTP_INTERNAL_SERVER_ERROR);
	Response response(HTTP_NO_CONTENT);
	return send(response.createMessage());
}

ft_bool	Worker::redirect(const std::string &dest) {
	Response response(HTTP_MOVED_PERMANENTLY);
	response.appendHeader("location", dest);
	return send(response.createMessage());
}

void	Worker::initRequestState() {
	isHeaderSet_ = FT_FALSE;
	isRecvCompleted_ = FT_FALSE;
}

ft_bool Worker::send(const std::vector<char> &message) {
	int							ret;
	std::vector<char>			m = message;
	std::vector<char>::iterator	it = m.begin();

	while (it != m.end()) {
		ret = ::send(pollfd_->fd, &(*it), 1, 0);
		if (ret == FT_ERROR)
			throw HttpException("send: send() failed", HTTP_INTERNAL_SERVER_ERROR);
		it++;
	}
	if (request_->getHeaderValue("connection") == "close") {
		std::cout << "connention: close" << std::endl;
		return FT_FALSE;
	}
	return FT_TRUE;
}

void Worker::resetPollfd() {
	close(pollfd_->fd);
	pollfd_->fd = -1;
	pollfd_->revents = 0;
}
