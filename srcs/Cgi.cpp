#include "Cgi.hpp"

Cgi::Cgi() {}

Cgi::Cgi(Request *request)
	: request_(request) {
	setEnv();
}

Cgi::~Cgi() {}

void Cgi::setEnv() {
	std::string uri = request_->getUri()->getParsedUri();
	std::string contentLength = request_->getHeaderValue("content-length");

	if (contentLength.length() <= 0)
		contentLength = ntos(request_->getBody().size());
	std::cout << "CONTENT_LENGTH: " << contentLength << std::endl;
	env_.push_back("CONTENT_LENGTH=" + contentLength);
	env_.push_back("CONTENT_TYPE=" + request_->getHeaderValue("content-type"));

	env_.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env_.push_back("PATH_INFO=" + uri + request_->getUri()->getPathInfo());
	env_.push_back("PATH_TRANSLATED=" + request_->getUri()->getFilePath());
	env_.push_back("QUERY_STRING=" + request_->getUri()->getQueryString());

	env_.push_back("REMOTE_ADDR=127.0.0.1");
	env_.push_back("REQUEST_METHOD=" + request_->getMethod());
	env_.push_back("REQUEST_URI=" + request_->getUri()->getOriginalUri());

	env_.push_back("SCRIPT_NAME=" + uri);

	std::string	host = request_->getHeaderValue("host");
	std::size_t	index = host.find(":");
	std::string	serverName = host;
	std::string	serverPort = ntos(request_->getLocationBlock().getPort());
	if (index != std::string::npos)
		serverName = host.substr(0, index);
	env_.push_back("SERVER_NAME=" + serverName);
	env_.push_back("SERVER_PORT=" + serverPort);
	env_.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_.push_back("SERVER_SOFTWARE=ngin-xs");

	std::map<std::string, std::string> requestHeaders = request_->getHeaders();
	std::map<std::string, std::string>::iterator it;

	for (it = requestHeaders.begin(); it != requestHeaders.end(); it++) {
		std::string field = it->first;
		std::transform(field.begin(), field.end(), field.begin(), ::toupper);
		std::size_t index;

		index = field.find("-");
		while (index != std::string::npos) {
			field.replace(index, 1, "_");
			index = field.find("-");
		}
		env_.push_back("HTTP_" + field + "=" + it->second);
	}
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

std::string Cgi::execute() {
	pid_t		pid;
	int			tmpStd[2];
	long		fileFds[2];
	std::string	result;
	int			ret = 1;
	std::vector<char> body = request_->getBody();
	char		readBuf[CGI_BUF_SIZE];

	tmpStd[STDIN_FILENO] = dup(STDIN_FILENO);
	tmpStd[STDOUT_FILENO] = dup(STDOUT_FILENO);

	FILE *tmpFileIn = tmpfile();
	FILE *tmpFileOut = tmpfile();
	fileFds[STDIN_FILENO] = fileno(tmpFileIn);
	fileFds[STDOUT_FILENO] = fileno(tmpFileOut);

	write(fileFds[STDIN_FILENO], reinterpret_cast<char*> (&body[0]), body.size());
	lseek(fileFds[STDIN_FILENO], 0, SEEK_SET);

	pid = fork();
	if (pid < 0) {
		std::cerr << "Fork crashed." << std::endl;
		result += "Status: ";
		result += HTTP_INTERNAL_SERVER_ERROR;
		result += EMPTY_LINE;
		return result;
	}

	/* child process */	
	if (pid == 0) {
		dup2(fileFds[STDIN_FILENO], STDIN_FILENO);
		dup2(fileFds[STDOUT_FILENO], STDOUT_FILENO);

		ret = execve(request_->getLocationBlock().getCgi().c_str(), NULL, getEnv());
		
		/* if execve() failed */
		std::cerr << "execute: CGI execve fail" << std::endl;		
		result += "Status: ";
		result += HTTP_INTERNAL_SERVER_ERROR;
		result += EMPTY_LINE;
		write(STDOUT_FILENO, result.c_str(), result.length());
	} else {
		waitpid(pid, NULL, 0);
		lseek(fileFds[STDOUT_FILENO], 0, SEEK_SET);
		while (ret > 0) {
			memset(readBuf, 0, CGI_BUF_SIZE);
			ret = read(fileFds[STDOUT_FILENO], readBuf, CGI_BUF_SIZE - 1);
			result += readBuf;
		}
	}

	/* parent process */
	dup2(tmpStd[STDIN_FILENO], STDIN_FILENO);
	dup2(tmpStd[STDOUT_FILENO], STDOUT_FILENO);

	fclose(tmpFileIn);
	fclose(tmpFileOut);
	close(fileFds[STDIN_FILENO]);
	close(fileFds[STDOUT_FILENO]);
	close(tmpStd[STDIN_FILENO]);
	close(tmpStd[STDOUT_FILENO]);

	return (result);
}
