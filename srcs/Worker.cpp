#include "Worker.hpp"

Worker::HttpException::HttpException(const std::string message, const std::string httpStatus)
	: message_(message), httpStatus_(httpStatus) {
		httpCode_ = atoi(httpStatus_.substr(0, 3).c_str());
	}

Worker::HttpException::~HttpException() throw() {}

const char *Worker::HttpException::what() const throw() {
	return message_.c_str();
}

const std::string &Worker::HttpException::getHttpStatus() {
	return httpStatus_;
}

int Worker::HttpException::getHttpCode() {
	return httpCode_;
}

std::vector<char> Worker::HttpException::makeErrorHtml(const std::string &errorPage) {
	std::string html = "<!DOCTYPE html>\n\
<html>\n\
<head>\n\
	<meta charset=\"UTF-8\">\n\
	<title>" + httpStatus_ + "</title>\n\
</head>\n\
<body>\n\
	<h1>" + httpStatus_ + "</h1>\n\
	<hr />\n\
	ngin-xs\n\
</body>\n\
</html>";
	if (errorPage != "" && isFileExist(errorPage))
		html = fileToString(errorPage);
	return stringToCharV(html);
}

Worker::Worker() {}

Worker::Worker(int listenSocket, const Block &serverBlock)
	: serverBlock_(serverBlock), request_(nullptr), isHeaderSet_(FT_FALSE), isRecvCompleted_(FT_FALSE), isNewRequest_(FT_TRUE), bodyLength_(0) {
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
			// TODO: redirection
			request_->setFilePath();
			std::string	requestMethod = request_->getMethod();
			if (requestMethod == "GET")
				return executeGet();
			if (requestMethod == "POST")
				return executePost();
			if (requestMethod == "DELETE")
				return executeDelete();
		}
		return ret;
	} catch (HttpException &e) {
		std::cerr << e.what() << std::endl;
		tmp_isRecvCompleted = isRecvCompleted_;
		initRequestState();
		Response response(e.getHttpStatus(), e.makeErrorHtml(request_->getLocationBlock().getErrorPage(e.getHttpCode())));
		return send(response.createMessage()) && tmp_isRecvCompleted;
	} catch (std::exception &e) {
		HttpException ex = HttpException(e.what(), HTTP_INTERNAL_SERVER_ERROR);
		std::cerr << "std::exception: " << e.what() << std::endl;
		tmp_isRecvCompleted = isRecvCompleted_;
		initRequestState();
		Response response(ex.getHttpStatus(), ex.makeErrorHtml(request_->getLocationBlock().getErrorPage(ex.getHttpCode())));
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
	const char 					*chunkedEnd = CHUNKED_END;
	std::vector<char>::iterator it;
	std::size_t					client_max_body_size;
	std::vector<char>::iterator	tmpIt;

	memset(buf, 0, BUFFER_LENGTH);
	ret = ::recv(pollfd_->fd, buf, BUFFER_LENGTH - 1, 0);
	buf[ret] = '\0';
	if (isNewRequest_ == FT_TRUE) {
		delete request_;
		request_ = new Request();
		isNewRequest_ = FT_FALSE;
	}
	if (isHeaderSet_ == FT_FALSE) {
		bodyLength_ += ret;
		request_->appendOriginalHeader(buf, ret);
		originalHeader = request_->getOriginalHeader();
		it = std::search(originalHeader.begin(), originalHeader.end(), crlf, crlf + strlen(crlf));
		if (it != originalHeader.end()) {
			isHeaderSet_ = FT_TRUE;
			request_->setOriginalBody(std::vector<char>(it + strlen(EMPTY_LINE), originalHeader.end()));
			request_->setOriginalHeader(std::vector<char>(originalHeader.begin(), it));
			bodyLength_ = bodyLength_ - request_->getOriginalHeader().size() - strlen(EMPTY_LINE);
			request_->setHeaders();
			Block locationBlock = serverBlock_.getLocationBlock(request_->getUri()->getParsedUri());
			if (locationBlock.getUri() == "")
				throw HttpException("recv: Location block not found", HTTP_NOT_FOUND);
			request_->setLocationBlock(locationBlock);
			client_max_body_size = request_->getLocationBlock().getClientMaxBodySize();
			transferEncoding = request_->getHeaderValue("transfer-encoding");
			if (transferEncoding.length() > 0 && transferEncoding != "chunked")
				throw HttpException("Not Implemented", HTTP_NOT_IMPLEMENTED);
			if (transferEncoding == "chunked" && request_->getHeaderValue("content-length").length() > 0)
				throw HttpException("recv: Chunked message with content length", HTTP_BAD_REQUEST);
			originalBody = request_->getOriginalBody();
			if ((request_->getContentLengthNumber() > client_max_body_size)
				|| (originalBody.size() > client_max_body_size))
				throw HttpException("recv: Content-length is larger than client max body size", HTTP_PAYLOAD_TOO_LARGE);
			if (ret < BUFFER_LENGTH && transferEncoding != "chunked"
				&& bodyLength_ >= request_->getContentLengthNumber())
				isRecvCompleted_ = FT_TRUE;
			it = std::search(originalBody.begin(), originalBody.end(), chunkedEnd, chunkedEnd + strlen(chunkedEnd));
			if (transferEncoding == "chunked" && it != originalBody.end())
				isRecvCompleted_ = FT_TRUE;
		}
	} else if (isRecvCompleted_ == FT_FALSE) {
		request_->appendOriginalBody(buf, ret);
		bodyLength_ += ret;
		originalBody = request_->getOriginalBody();
		transferEncoding = request_->getHeaderValue("transfer-encoding");
		client_max_body_size = request_->getLocationBlock().getClientMaxBodySize();
		if (transferEncoding == "chunked") {
			if (bodyLength_ > client_max_body_size)
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
		std::cout << "recv: Client finished connection." << std::endl;
		return FT_FALSE;
	}
	if (ret == FT_ERROR)
		throw HttpException("recv: fail", HTTP_INTERNAL_SERVER_ERROR);
	return FT_TRUE;
}

void Worker::validate() {
	std::set<std::string>					allowedMethods = request_->getLocationBlock().getAllowedMethods();
	std::set<std::string>::const_iterator	it = allowedMethods.find(request_->getMethod());

	if (it == allowedMethods.end())
		throw HttpException("validate: Invalid method", HTTP_METHOD_NOT_ALLOWED);
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
	std::string indexPath = request_->getLocationBlock().getIndex();
	ft_bool	isAutoindex = request_->getLocationBlock().getAutoIndex();
	if (isFileExist(filePath + indexPath))
		isAutoindex = FT_FALSE;
	if (isDirectory(filePath) && isAutoindex) {
		Autoindex autoindex(filePath, request_->getUri()->getOriginalUri());
		Response response(HTTP_OK, stringToCharV(autoindex.getHtml()));
		return send(response.createMessage());
	}
	if (isDirectory(filePath))
		filePath += indexPath;
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
	if (request_->getLocationBlock().getCgi().length() <= 0)
		throw HttpException("executePost: Invaild POST request", HTTP_NOT_FOUND);
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
	isNewRequest_ = FT_TRUE;
}

ft_bool	Worker::isCgi(const std::string &filePath) {
	std::set<std::string>			supportedExtensions = request_->getLocationBlock().getSupportedExtensions();
	std::set<std::string>::iterator	it;
	std::string						extension;
	int								len;
	int								i;

	len = filePath.length();
	i = filePath.find_last_of('.');
	if (i <= 0 || i == len - 1)
		return FT_FALSE;
	extension = filePath.substr(i + 1);
	for (it = supportedExtensions.begin(); it != supportedExtensions.end(); it++) {
		if (extension == *it)
			return FT_TRUE;
	}
	return FT_FALSE;
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
