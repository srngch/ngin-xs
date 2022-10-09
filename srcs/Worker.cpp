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
	socklen_t			addressLen;

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

std::string	Worker::executeCgiProgram(const std::string &filePath) {
	char	*args[3];
	pid_t	pid;
	int		infile;
	int		outfile;
	int		status;

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
	pid = fork();
	if (pid < 0) {
		throw std::runtime_error("Fork failed");
	} else if (pid == 0) {
		if (dup2(infile, STDIN_FILENO) == FT_ERROR)
			exit(EXIT_FAILURE);
		if (dup2(outfile, STDOUT_FILENO) == FT_ERROR)
			exit(EXIT_FAILURE);
		args[0] = const_cast<char *>(std::string(PYTHON_PATH).c_str());
		args[1] = const_cast<char *>(filePath.c_str());
		args[2] = nullptr;
		if (execve(PYTHON_PATH, args, nullptr) == FT_ERROR) // TODO: set env
			exit(EXIT_FAILURE);
	}
	waitpid(pid, &status, 0);
	close(infile);
	close(outfile);
	if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
		throw std::runtime_error("CGI program execution failed");
	return fileToString(CGI_OUTFILE);
}

std::string Worker::autoindex(const std::string &filePath) {
	DIR						*dir;
	struct dirent			*entry;
	Autoindex				ai;
	std::string				fullPath;
	struct stat				fileStat;
	struct tm				*tm;
	char					dateBuf[20];
	std::vector<Autoindex>	autoindexes;
	std::string				result;
	std::vector<Autoindex>::iterator	it;

	dir = opendir(filePath.c_str());
	if (!dir)
		throw std::runtime_error("autoindex: Directory opening failed");
	while ((entry = readdir(dir)) != nullptr) {
		ai.setName(entry->d_name);
		ai.setIsDirectory(FT_FALSE);
		fullPath = filePath + "/" + entry->d_name;
		if (isDirectory(fullPath)) {
			ai.setName(ai.getName() + "/");
			ai.setIsDirectory(FT_TRUE);
		}
		if (ai.getName().length() > 50) {
			ai.setName(ai.getName().substr(0, 47));
			ai.setName(ai.getName() + "..>");
		}
		stat(fullPath.c_str(), &fileStat);
		tm = gmtime(&fileStat.st_mtime);
		strftime(dateBuf, 20, "%d-%b-%Y %H:%M", tm);
		ai.setDate(dateBuf);
		ai.setSize(fileStat.st_size);
		ai.setPath(request_->getUri() + entry->d_name);
		if (ai.getIsDirectory())
			ai.setPath(ai.getPath() + "/");
		if (ai.getName() != "./")
			autoindexes.push_back(ai);
	}
	std::sort(autoindexes.begin(), autoindexes.end(), sortAutoindexes);
	closedir(dir);
	result += "<html> \
			<head> \
				<title>Index of " + request_->getUri() + "</title> \
			</head> \
			<body> \
				<h1>Index of " + request_->getUri() + "</h1> \
				<hr> \
				<pre>";
	for (it = autoindexes.begin(); it != autoindexes.end(); it++) {
		result += "<a href=\"" + it->getPath() + "\">" \
			+ it->getName() + "</a>" \
			+ createPadding(50, it->getName().length()) + " " \
			+ it->getDate() + " ";
		if (it->getIsDirectory())
			result += createPaddedString(19, "-");
		else
			result += createPaddedString(19, ntos(it->getSize()));
		result += "\n";
	}
	result += "</pre> \
				<hr> \
			</body> \
		</html>";
	return result;
}

ft_bool Worker::work() {
	int		ret = FT_TRUE;
	std::string	result;

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
				// if (isDirectory(filePath)) {	// TODO: autoindex 설정이 없는지 체크
				// 	filePath += "/index.html"; // TODO: read default file from config
				// }
				if (isCgi(filePath)) {
					std::string outFile = executeCgiProgram(filePath);
					Response response(HTTP_OK, outFile);
					send(response.createMessage().c_str());
					remove(CGI_INFILE);
					remove(CGI_OUTFILE);
					return ret;
				}
				if (isDirectory(filePath) && request_->getUri().substr(0, 7) == "/upload")	// TODO: autoindex 설정이 있는지 체크
					result = autoindex(filePath);
				else if (isFileExist(filePath))
					result = fileToString(filePath);
				else
					throw FileNotFoundException("File not found");
				Response response(HTTP_OK, result);
				send(response.createMessage().c_str());
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
