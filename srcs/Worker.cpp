#include "Worker.hpp"

extern timeval start;

Worker::HttpException::HttpException(const std::string &message, const std::string &httpStatus)
	: message_(message), httpStatus_(httpStatus) {
		httpCode_ = atoi(httpStatus_.substr(0, 3).c_str());
	}

Worker::HttpException::~HttpException() throw() {}

const char *Worker::HttpException::what() const throw() {
	return message_.c_str();
}

vectorChar Worker::HttpException::makeErrorHtml(const std::string &errorPage) {
	std::string	html;

	if (httpCode_ == 204)
		return stringToCharV(html);
	if (errorPage != "" && isFileExist(errorPage))
		return fileToCharV(errorPage);
	html = "<!DOCTYPE html>\n\
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
	return stringToCharV(html);
}

const std::string &Worker::HttpException::getHttpStatus() {
	return httpStatus_;
}

int Worker::HttpException::getHttpCode() {
	return httpCode_;
}

Worker::Worker(int listenSocket, const Block &serverBlock)
	: serverBlock_(serverBlock), request_(nullptr), isHeaderSet_(FT_FALSE), isRecvCompleted_(FT_FALSE), isNewRequest_(FT_TRUE) {
	struct sockaddr_in	clientAddress;
	socklen_t			addressLen = sizeof(clientAddress);

	connectSocket_ = accept(listenSocket, (struct sockaddr *)&clientAddress, &addressLen);
	std::cout << "connectSocket_: " << connectSocket_ << std::endl;
	if (connectSocket_ == -1) {
		close(listenSocket);
		throw std::runtime_error("Worker constructor: accept() failed");
	}
}

Worker::~Worker() {
	resetPollfd();
	delete request_;
}

ft_bool Worker::recv() {
	int 			ret;
	char			buf[RECV_BUF_SIZE];
	std::string		transferEncoding;
	vectorChar		originalHeader;
	vectorChar		originalBody;
	const char 		*emptyLine = EMPTY_LINE;
	const char 		*chunkedEnd = CHUNKED_END;
	vectorCharIter	it;
	std::size_t		client_max_body_size;

	memset(buf, 0, RECV_BUF_SIZE);
	ret = ::recv(pollfd_->fd, buf, RECV_BUF_SIZE - 1, 0);
	if (ret == FT_FALSE) {
		resetPollfd();
		std::cout << "recv: Client finished connection." << std::endl;
		return FT_FALSE;
	}
	if (ret == FT_ERROR)
		throw HttpException("recv: recv() failed", HTTP_INTERNAL_SERVER_ERROR);
	buf[ret] = '\0';
	if (isNewRequest_ == FT_TRUE) {
		timestamp("recv start", start, connectSocket_);
		delete request_;
		request_ = new Request();
		isNewRequest_ = FT_FALSE;
	}
	if (isHeaderSet_ == FT_FALSE) {
		request_->appendOriginalHeader(buf, ret);
		request_->addBodyLength(ret);
		originalHeader = request_->getOriginalHeader();
		it = std::search(originalHeader.begin(), originalHeader.end(), emptyLine, emptyLine + strlen(emptyLine));
		if (it != originalHeader.end()) {
			// start reading body
			isHeaderSet_ = FT_TRUE;
			request_->setOriginalBody(it + strlen(EMPTY_LINE), originalHeader.end());
			request_->setOriginalHeader(originalHeader.begin(), it);
			request_->setHeaders();
			// 현재 요청에 대한 conf 파일의 location block 가져오기
			Block locationBlock = serverBlock_.getLocationBlock(request_->getUri()->getParsedUri());
			if (locationBlock.getUri() == "")
				throw HttpException("recv: Location block not found", HTTP_NOT_FOUND);
			request_->setLocationBlock(locationBlock);
			std::cerr << "======URI: " << request_->getUri()->getOriginalUri() << std::endl;
			client_max_body_size = request_->getLocationBlock().getClientMaxBodySize();

			transferEncoding = request_->getHeaderValue("transfer-encoding");
			// transfer-encoding 헤더의 값이 chunked가 아닌 것은 구현하지 않음
			if (transferEncoding.length() > 0 && transferEncoding != "chunked")
				throw HttpException("recv: Not implemented transfer-encoding", HTTP_NOT_IMPLEMENTED);

			originalBody = request_->getOriginalBody();
			if (transferEncoding == "chunked") {
				// content-length 헤더가 있다면 400 응답
				if (request_->getHeaderValue("content-length").length() > 0)
					throw HttpException("recv: Chunked message with content-length", HTTP_BAD_REQUEST);
				request_->setBodyLength(0);
				request_->parseChunkedBody();
				// body가 client_max_body_size 보다 큼
				if (request_->getBodyLength() > client_max_body_size)
					throw HttpException("recv: Chunked request body is larger than client max body size", HTTP_CONTENT_TOO_LARGE);
				// CRLFCRLF를 체크해서 request 내용의 전체를 받았는지 확인
				if (hasWordInCharV(originalBody, chunkedEnd))
					isRecvCompleted_ = FT_TRUE;
			} else {
				request_->setBodyLength(request_->getBodyLength() - request_->getOriginalHeader().size() - strlen(EMPTY_LINE));
				// content-length 헤더가 client_max_body_size 보다 큼
				if (request_->getContentLengthNumber() > client_max_body_size
					|| originalBody.size() > client_max_body_size)
					throw HttpException("recv: Content-length is larger than client max body size", HTTP_CONTENT_TOO_LARGE);
				// 전체 메시지 길이가 버퍼보다 작을 때, 내용의 전체를 다 받았는지 확인
				if (ret < RECV_BUF_SIZE && request_->getBodyLength() >= request_->getContentLengthNumber())
					isRecvCompleted_ = FT_TRUE;
			}
		}
	} else if (isRecvCompleted_ == FT_FALSE) {
		request_->appendOriginalBody(buf, ret);
		transferEncoding = request_->getHeaderValue("transfer-encoding");
		client_max_body_size = request_->getLocationBlock().getClientMaxBodySize();
		if (transferEncoding == "chunked") {
			request_->parseChunkedBody();
			if (request_->getBodyLength() > client_max_body_size)
				throw HttpException("recv: Chunked request body is larger than client max body size", HTTP_CONTENT_TOO_LARGE);
			// CRLFCRLF를 체크해서 request 내용의 전체를 받았는지 확인
			if (hasWordInCharV(request_->getOriginalBody(), chunkedEnd))
				isRecvCompleted_ = FT_TRUE;
		} else {
			request_->addBodyLength(ret);
			if (request_->getBodyLength() >= request_->getContentLengthNumber())
				isRecvCompleted_ = FT_TRUE;
		}
	}
	return FT_TRUE;
}

void Worker::validate() {
	setString			allowedMethods = request_->getLocationBlock().getAllowedMethods();
	setStringConstIter	it = allowedMethods.find(request_->getMethod());

	if (it == allowedMethods.end())
		throw HttpException("validate: Invalid method", HTTP_METHOD_NOT_ALLOWED);
	if (request_->getVersion() != "HTTP/1.1")
		throw HttpException("validate: HTTP version is not 1.1", HTTP_VERSION_NOT_SUPPORTED);
	if (request_->getHeaderValue("host").length() == 0)
		throw HttpException("validate: Host header does not exist", HTTP_BAD_REQUEST);
}

ft_bool Worker::executePutToTest() {
	Response response(HTTP_CREATED, stringToCharV("PUT success"));
	return send(response.createMessage());
}

ft_bool Worker::executeGet() {
	std::string	filePath;
	std::size_t	index;
	std::string	extension;

	filePath = request_->getFilePath();
	if (isDirectory(filePath) && request_->getUri()->getBaseName().length() > 0) {
		redirect(request_->getUri()->getParsedUri() + request_->getUri()->getPathInfo() + "/" + request_->getUri()->getQueryString());
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

		Response response(HTTP_OK, cgi.execute(), FT_TRUE);
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
		throw HttpException("executePost: Invaild POST request", HTTP_NO_CONTENT);
	if (isCgi(request_->getFilePath())) {
		Cgi cgi(request_);
		timestamp("response make start", start, connectSocket_);
		Response response(HTTP_CREATED, cgi.execute(), FT_TRUE);	// stringToCharV 오래 걸림 (1103635)
		timestamp("response make end", start, connectSocket_);
		timestamp("createMessage start", start, connectSocket_);
		return send(response.createMessage());	// createMessage 오래 걸림 (1055090)
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

ft_bool Worker::redirect(const std::string &dest) {
	Response response(HTTP_MOVED_PERMANENTLY);
	response.appendHeader("location", dest);
	return send(response.createMessage());
}

void Worker::initRequestState() {
	isHeaderSet_ = FT_FALSE;
	isRecvCompleted_ = FT_FALSE;
	isNewRequest_ = FT_TRUE;
}

ft_bool Worker::isCgi(const std::string &filePath) {
	setString		supportedExtensions = request_->getLocationBlock().getSupportedExtensions();
	setStringIter	it;
	std::string		extension;
	int				len;
	int				i;

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

ft_bool Worker::send(const vectorChar &message) {
	int			ret;
	vectorChar	m = message;

	timestamp("Send start", start, connectSocket_);
	ret = ::send(pollfd_->fd, reinterpret_cast<char*>(&m[0]), m.size(), 0);
	timestamp("Send end", start, connectSocket_);
	std::cerr << "===================" << std::endl;
	if (ret == FT_ERROR)
		throw HttpException("send: send() failed", HTTP_INTERNAL_SERVER_ERROR);
	if (request_->getHeaderValue("connection") == "close")
		return FT_FALSE;
	return FT_TRUE;
}

void Worker::resetPollfd() {
	close(pollfd_->fd);
	pollfd_->fd = -1;
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
			std::cout << "<socket closed>" << std::endl;
			return FT_FALSE;
		} else if (pollfd_->revents == pollfd_->events) {
			ret = recv();
			if (ret == FT_FALSE)
				return ret;
		} else if (pollfd_->revents == POLLOUT && isRecvCompleted_ == FT_TRUE) {
			timestamp("recv end", start, connectSocket_);
			timestamp("setBody start", start, connectSocket_);
			request_->setBody();
			timestamp("setBody end", start, connectSocket_);
			initRequestState();
			validate();
			request_->setFilePath();
			std::string	requestMethod = request_->getMethod();
			if (requestMethod == "PUT")
				return executePutToTest();
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

void Worker::setPollfd(struct pollfd *pollfd) {
	pollfd_ = pollfd;
	pollfd_->fd = connectSocket_;
	pollfd_->events = POLLIN | POLLOUT;
	pollfd_->revents = 0;
}
