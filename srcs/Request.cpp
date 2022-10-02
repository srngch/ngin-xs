#include "Request.hpp"
#include <typeinfo> // TEST

// POST /v1/login HTTP/1.1\r\n
// Content-Type: application/json; charset=utf-8\r\n
// Host: 127.0.0.1:8000\r\n
// Connection: close\r\n
// User-Agent: Paw/3.4.0 (Macintosh; OS X/12.6.0) GCDHTTPRequest\r\n
// Content-Length: 55\r\n
// \r\n
// {"username":"test","hashed_password":"hashed_password"}

Request::Request(const std::string &originalMessage) {
	std::vector<std::string> splitedMessage;
	
	splitedMessage = split(originalMessage, CRLF);
	parse(splitedMessage);

	std::cout << "[parsing result]" << std::endl;
	std::cout << "method_: " << method_ << "|" << typeid(method_).name()<< std::endl;
	std::cout << "uri_: " << uri_ << "|" << typeid(uri_).name() << std::endl;
	std::cout << "version_: " << version_ << std::endl;
	
	std::cout << "===============" << std::endl;

	std::multimap<std::string, std::string>::iterator iter;
	for (iter = headers_.begin(); iter != headers_.end(); iter++)
		std::cout << iter->first << " | " << iter->second << std::endl;
	
	std::cout << "===============" << std::endl;
	std::cout << "body_: " << body_ << std::endl;
}

Request::~Request() {}

// ft_bool Request::validate() {

// }

void	Request::parse(const std::vector<std::string> &splitedMessage) {
	std::vector<const std::string>::iterator	iter = splitedMessage.begin();
	std::vector<std::string> splitedRequstLine = split(*iter, " ");
	std::vector<std::string> splitedHeaderLine;

	// request line
	if (splitedRequstLine.size() != 3)
		throw std::runtime_error("parse error");
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

const std::string &Request::getMethod() {
	return method_;
}

const std::string &Request::getUri() {
	return uri_;
}

const std::string &Request::getVersion() {
	return version_;
}

const std::multimap<std::string, std::string> &Request::getHeaders() {
	return headers_;
}

const std::vector<std::string>	*Request::getHeaderValues(std::string fieldName) {
	std::vector<std::string> *headerValues = new std::vector<std::string>;
	std::pair <std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> ret;

	ret = headers_.equal_range(fieldName);
	for (
		std::multimap<std::string, std::string>::iterator it=ret.first;
		it!=ret.second;
		++it
	)
	headerValues->push_back(it->second);

	return headerValues;
}

const std::string &Request::getBody() {
	return body_;
}
