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

Request::Request() {}

Request::Request(const std::string &header) {
	std::vector<std::string>						splitedMessage;
	std::map<std::string, std::string>::iterator	iter;
	
	splitedMessage = split(header, CRLF);
	parseHeader(splitedMessage);

	for (iter = headers_.begin(); iter != headers_.end(); iter++)
		std::cout << iter->first << " | " << iter->second << std::endl;
}

Request::~Request() {
	delete uri_;
}

void	Request::parseHeader(const std::vector<std::string> &splitedMessage) {
	std::vector<const std::string>::iterator	iter = splitedMessage.begin();
	std::vector<std::string> splitedRequstLine = split(*iter, " ");
	std::vector<std::string> splitedHeaderLine;

	// request line
	if (splitedRequstLine.size() != 3)
		throw std::runtime_error("parse error");
	method_ = splitedRequstLine[0];
	uri_ = new Uri(splitedRequstLine[1]);
	version_ = splitedRequstLine[2];
	iter++;
	// headers
	while (iter != splitedMessage.end()) {
		splitedHeaderLine = split(*iter, ": ");
		std::transform(splitedHeaderLine[0].begin(), splitedHeaderLine[0].end(), splitedHeaderLine[0].begin(), ::tolower);
		headers_.insert(std::pair<std::string, std::string>(splitedHeaderLine[0], splitedHeaderLine[1]));
		iter++;
	}
}

void	Request::parseChunkedBody(const std::string &originalBody) {
	std::vector<std::string>			splitedBody = split(originalBody, CRLF);
	std::vector<std::string>::iterator	it;
	size_t								length;

	for (it = splitedBody.begin(); it != splitedBody.end(); it++) {
		length = hexStringToNumber(*it);
		if (length == 0)
			break;
		it++;
		body_ += it->substr(0, length);
	}
}

const std::string &Request::getMethod() {
	return method_;
}

const Uri *Request::getUri() {
	return uri_;
}

const std::string &Request::getVersion() {
	return version_;
}

const std::map<std::string, std::string> &Request::getHeaders() {
	return headers_;
}

const std::string Request::getHeaderValue(std::string fieldName) {
	std::map<std::string, std::string>::iterator it = headers_.find(fieldName);
	if (it == headers_.end())
		return std::string("");
	return it->second;
}

const std::string &Request::getBody() {
	return body_;
}

void	Request::setBody(std::string &body) {
	if (getHeaderValue("transfer-encoding") == "chunked") {
		parseChunkedBody(body);
	} else {
		std::size_t contentLength = atoi(getHeaderValue("content-length").c_str());
		body_ = body.substr(0, contentLength);
	}
	// std::cout << "==============" << std::endl;
	// std::cout << "body_: " << body_ << std::endl;
}

void	Request::appendHeader(std::string fieldName, std::string value) {
	headers_.insert(std::pair<std::string, std::string>(fieldName, value));
}
