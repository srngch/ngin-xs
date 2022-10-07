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

Worker::Worker(std::vector<std::string> env, int listenSocket) {
	struct sockaddr_in	clientAddress;
	socklen_t			addressLen;

	env_ = env;
	addressLen = sizeof(clientAddress);
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

char **Worker::makeCgiEnv() {
// AUTH_TYPE
// CONTENT_LENGTH
// CONTENT_TYPE

// GATEWAY_INTERFACE
// PATH_INFO
// PATH_TRANSLATED
// QUERY_STRING

// REMOTE_ADDR
// REMOTE_IDENT
// REMOTE_USER
// REQUEST_METHOD
// REQUEST_URI

// SCRIPT_NAME

// SERVER_NAME
// SERVER_PORT
// SERVER_PROTOCOL
// SERVER_SOFTWARE
	std::cout << "makeCgiEnv" << std::endl;

	// std::cout << "content_length: " << (*request_->getHeaderValues("content-length"))[0] << std::endl;
	// CGI 헤더 추가
	if (request_->getHeaderValues("content-length")->size() != 0)
		env_.push_back("CONTENT_LENGTH=" + (*request_->getHeaderValues("content-length"))[0]);
	if (request_->getHeaderValues("content-type")->size() != 0)
		env_.push_back("CONTENT_TYPE=" + (*request_->getHeaderValues("content-type"))[0]);

	env_.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env_.push_back("PATH_INFO=" + request_->getUri());
	env_.push_back("PATH_TRANSLATED=");
	// if (request_->getQueryString())
		// env_.push_back("QUERY_STRING=");

	env_.push_back("REMOTE_ADDR=");
	env_.push_back("REMOTE_IDENT=");
	env_.push_back("REMOTE_USER=");
	env_.push_back("REQUEST_METHOD=" + request_->getMethod());
	env_.push_back("REQUEST_URI=");
	
	env_.push_back("SCRIPT_NAME=");

	// Host: localhost:8080
	env_.push_back("SERVER_NAME=localhost");
	env_.push_back("SERVER_PORT=4242");
	env_.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_.push_back("SERVER_SOFTWARE=ngin-xs");
	

	return getEnv();
	
	// HTTP_ACCEPT=request_.getMethod()
	// GATEWAY_INTERFACE=CGI/1.1
	// SERVER_PROTOCOL=HTTP/1.1

	// ? HTTP_ACCEPT_CHARSET
	// ? HTTP_ACCEPT_ENCODING
	// HTTP_HOST= (요청 메시지에 있는 host 헤더)
	// HTTP_USER_AGENT (요청 메시지에 있는 user agent)
	// HTTP_COOKIE

	// QUERY_STRING (요청 URL의 쿼리 스트링)

	// SCRIPT_NAME
	// REMOTE_ADDR (요청을 보낸 클라이언트 IP 주소)
	// REMOTE_HOST
	// REMOTE_USER
	// REQUEST_METHOD (요청 메시지에 있는 메소드)

	// SERVER_NAME (서버의 호스트 네임)
	// SERVER_PORT (서버의 포트 번호)
	// SERVER_SOFTWARE
}

char **Worker::getEnv() {
	char	**charEnv;
	size_t	i;

	charEnv = (char **)malloc(sizeof(char *) * (env_.size() + 1));
	for (i = 0; i < env_.size(); i++)
		charEnv[i] = strdup(env_[i].c_str());
	charEnv[i] = nullptr;
	return charEnv;
}

std::string	Worker::executeCgiProgram(const std::string &filePath) {
	char	*args[3];
	pid_t	pid;
	int		infile;
	int		outfile;
	int		status;
	// int		tmpStdIn;
	// int		tmpStdOut;

	// tmpStdIn = dup(STDIN_FILENO);
	// tmpStdOut = dup(STDOUT_FILENO);
	if (access(filePath.c_str(), R_OK))
		throw ForbiddenException("Permission denied");
	if (!isFileExist(filePath))
		throw FileNotFoundException("File not found(cgi)");
	infile = open(CGI_INFILE, O_WRONLY | O_CREAT, CGI_FILE_MODE);
	if (infile == FT_ERROR)
		throw std::runtime_error("Infile open failed");
	write(infile, request_->getBody().c_str(), request_->getBody().length());
	outfile = open(CGI_OUTFILE, O_WRONLY | O_CREAT, CGI_FILE_MODE);
	if (outfile == FT_ERROR) {
		close(infile);
		throw std::runtime_error("Outfile open failed");
	}
	makeCgiEnv();
	pid = fork();
	if (pid < 0) {
		throw std::runtime_error("Fork failed");
	} else if (pid == 0) {
		if (dup2(infile, STDIN_FILENO) == FT_ERROR)
			exit(EXIT_FAILURE);
		if (dup2(outfile, STDOUT_FILENO) == FT_ERROR)
			exit(EXIT_FAILURE);
		args[0] = const_cast<char *>(std::string("/Users/hhkim/ngin-xs/cgi_tester").c_str());
		// args[1] = const_cast<char *>(filePath.c_str());
		args[1] = nullptr;
		// makeCgiEnv()
		// std::cout << "##############" << std::endl;
		// char **envs = makeCgiEnv();
		// while (envs && *envs) {
		// 	std::cout << *envs << std::endl;
		// 	envs++;
		// }
		// std::cout << "$$$$$$$$$$$$$" << std::endl;
		if (execve("/Users/hhkim/ngin-xs/cgi_tester", args, getEnv()) == FT_ERROR) // TODO: set env
			exit(EXIT_FAILURE);
	}
	waitpid(pid, &status, 0);
	// dup2(tmpStdIn, STDIN_FILENO);
	// dup2(tmpStdOut, STDOUT_FILENO);
	close(infile);
	close(outfile);
	if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
		throw std::runtime_error("CGI program execution failed");
	return fileToString(CGI_OUTFILE);
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
					std::string outFile = executeCgiProgram(filePath);
					Response response(HTTP_OK, outFile);
					send(response.createMessage().c_str());
					remove(CGI_INFILE);
					remove(CGI_OUTFILE);
					return ret;
				}
				if (isFileExist(filePath) == FT_FALSE)
					throw FileNotFoundException("File not found");
				Response response(HTTP_OK, fileToString(filePath));
				send(response.createMessage().c_str());
			}
			if (request_->getMethod() == "POST") {
				std::string filePath = std::string(WEB_ROOT) + request_->getUri();
				if (isCgi(filePath)) {
					std::string outFile = executeCgiProgram(filePath);
					Response response(HTTP_CREATED, outFile);
					send(response.createMessage().c_str());
					// remove(CGI_INFILE);
					// remove(CGI_OUTFILE);
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
