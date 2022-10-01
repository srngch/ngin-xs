#include "Request.hpp"

// POST /v1/login HTTP/1.1\r\n
// Content-Type: application/json; charset=utf-8\r\n
// Host: 127.0.0.1:8000\r\n
// Connection: close\r\n
// User-Agent: Paw/3.4.0 (Macintosh; OS X/12.6.0) GCDHTTPRequest\r\n
// Content-Length: 55\r\n
// \r\n
// {"username":"test","hashed_password":"hashed_password"}

#define CRLF "\r\n"

Request::Request(const std::string &originalMessage) {
	std::vector<std::string> splitedMessage;
	
	splitedMessage = split(originalMessage, CRLF);
	parse(splitedMessage);

	std::cout << "[parsing result]" << std::endl;
	std::cout << "method_: " << method_ << std::endl;
	std::cout << "uri_: " << uri_ << std::endl;
	std::cout << "version_: " << version_ << std::endl;
	
	std::cout << "===============" << std::endl;

	std::multimap<std::string, std::string>::iterator iter;
	for (iter = headers_.begin(); iter != headers_.end(); iter++)
		std::cout << iter->first << " | " << iter->second << std::endl;
	
	std::cout << "===============" << std::endl;
	std::cout << "body_: " << body_ << std::endl;
	// TODO: validate
}

Request::~Request() {}

void	Request::parse(const std::vector<std::string> &splitedMessage) {
	std::vector<const std::string>::iterator	iter = splitedMessage.begin();
	std::vector<std::string> splitedRequstLine = split(*iter, " ");
	std::vector<std::string> splitedHeaderLine;

	// request line
	method_ = splitedRequstLine[0];
	uri_ = splitedRequstLine[1];
	version_ = splitedRequstLine[2];
	iter++;
	// headers
	while (iter->length() != 0) {
		splitedHeaderLine = split(*iter, ": ");
		std::transform(splitedHeaderLine[0].begin(), splitedHeaderLine[0].end(), splitedHeaderLine[0].begin(), ::tolower);
		headers_.insert(std::pair<std::string, std::string>(splitedHeaderLine[0], splitedHeaderLine[1]));
		iter++;
	}
	// body
	iter++;
	body_ = *iter;
}

std::vector<std::string> Request::split(const std::string &str, std::string delim) {
	std::vector<std::string>	splitedMessage;
	std::size_t 				index;
	std::string					tmp = str;
	std::string					s;

	while (FT_TRUE) {
		if (tmp.find(delim) == 0)
			tmp = tmp.substr(delim.length(), std::string::npos);
		index = tmp.find(delim);
		s = tmp.substr(0, index);
		splitedMessage.push_back(s);
		if (index == std::string::npos)
			break ;
		tmp = tmp.substr(index, std::string::npos);
	}
	return splitedMessage;
}
