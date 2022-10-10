#include "Cgi.hpp"

Cgi::Cgi(Request *request) {
	request_ = request;
	setEnv();
}

Cgi::~Cgi() {}

void Cgi::setEnv() {
	// TODO: AUTH_TYPE
	// if (request_->getAuthType())
	// 	env_.push_back("AUTH_TYPE=" + request_->getAuthType());

	if (request_->getHeaderValues("content-length")->size() != 0)
		env_.push_back("CONTENT_LENGTH=" + (*request_->getHeaderValues("content-length"))[0]);
	if (request_->getHeaderValues("content-type")->size() != 0)
		env_.push_back("CONTENT_TYPE=" + (*request_->getHeaderValues("content-type"))[0]);

	env_.push_back("GATEWAY_INTERFACE=CGI/1.1");

	env_.push_back("PATH_INFO=" + request_->getUri()); // TODO: get pathInfo
	env_.push_back("PATH_TRANSLATED=" + std::string(WEB_ROOT) + request_->getUri());
	// if (request_->getQueryString())
		env_.push_back("QUERY_STRING="); // TODO: + request_->getQueryString()

	env_.push_back("REMOTE_ADDR=127.0.0.1");
	// env_.push_back("REMOTE_IDENT=");
	// env_.push_back("REMOTE_USER=");
	env_.push_back("REQUEST_METHOD=" + request_->getMethod());
	env_.push_back("REQUEST_URI=" + request_->getUri());

	// env_.push_back("SCRIPT_NAME=");

	env_.push_back("SERVER_NAME=localhost");
	env_.push_back("SERVER_PORT=4242");
	env_.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_.push_back("SERVER_SOFTWARE=ngin-xs");
	// HTTP_ACCEPT=request_.getMethod()
	// HTTP_HOST= (요청 메시지에 있는 host 헤더)
	// HTTP_USER_AGENT (요청 메시지에 있는 user agent)
	// HTTP_COOKIE
}

char **Cgi::getEnv() {
	char	**charEnv;
	size_t	i;

	charEnv = (char **)malloc(sizeof(char *) * (env_.size() + 1));
	for (i = 0; i < env_.size(); i++)
		charEnv[i] = strdup(env_[i].c_str());
	charEnv[i] = nullptr;
	return charEnv;
}

std::string	Cgi::execute() {
	pid_t		pid;
	int			writePipe[2];
	int			readPipe[2];
	int			devNull;
	char		readBuf[1024];
	std::string	result;

	if (pipe(writePipe) == FT_ERROR) {
		throw std::runtime_error("fail: pipe()");
	}
	if (pipe(readPipe) == FT_ERROR) {
		close(writePipe[0]);
		close(writePipe[1]);
		throw std::runtime_error("fail: pipe()");
	}

	pid = fork();
	if (pid < 0) {
		close(writePipe[0]);
		close(writePipe[1]);
		close(readPipe[0]);
		close(readPipe[1]);
		throw std::runtime_error("Fork failed");
	}

	/* child process */
	if (pid == 0) {
		close(writePipe[1]);
		close(readPipe[0]);

		/*
		** redirect
		** - stdin -> writePipe[0]
		** - stdout -> readPipe[1]
		** - stderr -> /dev/null
		*/
		if (dup2(writePipe[0], STDIN_FILENO) == FT_ERROR)
			exit(EXIT_FAILURE);
		close(writePipe[0]);
		if (dup2(readPipe[1], STDOUT_FILENO) == FT_ERROR)
			exit(EXIT_FAILURE);
		close(readPipe[1]);
		devNull = open("/dev/null", O_WRONLY);
		if (devNull == FT_ERROR)
			exit(EXIT_FAILURE);
		if (dup2(devNull, STDERR_FILENO) == FT_ERROR)
			exit(EXIT_FAILURE);

		execve(std::string(WEB_ROOT + request_->getUri()).c_str(), NULL, getEnv());

		/* if execve() failed */
		exit(EXIT_FAILURE);
	}

	/* parent process */
	close(writePipe[0]);
	close(readPipe[1]);

	/* if POST method, write request body to child process */
	if (request_->getMethod() == "POST") {
		write(writePipe[1], request_->getBody().c_str(), request_->getBody().size());
		// TODO: send eof
		write(writePipe[1], "\0", 1);
	}
	close(writePipe[1]);

	memset(readBuf, 0, sizeof(readBuf));
	while (read(readPipe[0], readBuf, 1024) > 0) {
		result += readBuf;
	}
	close(readPipe[0]);

	return result;
}
