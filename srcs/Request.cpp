#include "Request.hpp"

Request::Request()
	: uri_(nullptr) {}

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
	uri_ = new Uri(splitedRequstLine[1], locationBlock_.getSupportedExtensions());
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

void	Request::parseChunkedBody() {
	std::vector<std::string>			splitedBody = split(std::string(originalBody_.begin(), originalBody_.end()), CRLF);
	std::vector<std::string>::iterator	it;
	size_t								length;

	for (it = splitedBody.begin(); it != splitedBody.end(); it++) {
		length = hexStringToNumber(*it);
		if (length == 0)
			break;
		it++;
		body_.insert(body_.begin(), it->begin(), it->begin() + length);
	}
}

const Block &Request::getLocationBlock() {
	return locationBlock_;
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

const std::vector<char> &Request::getBody() {
	return body_;
}

const std::string &Request::getFilePath() {
	return filePath_;
}

const std::vector<char> &Request::getOriginalHeader() {
	return originalHeader_;
}

const std::vector<char> &Request::getOriginalBody() {
	return originalBody_;
}

void Request::setLocationBlock(const Block &locationBlock) {
	locationBlock_ = locationBlock;
}

std::size_t Request::getContentLengthNumber() {
	return atoi(getHeaderValue("content-length").c_str());
}


void	Request::setBody() {
	if (getHeaderValue("transfer-encoding") == "chunked") {
		parseChunkedBody();
	} else {
		std::size_t contentLength = getContentLengthNumber();
		body_ = std::vector<char>(originalBody_.begin(), originalBody_.begin() + contentLength);
	}
}

void Request::setHeaders() {
	std::vector<std::string>						splitedMessage;
	std::map<std::string, std::string>::iterator	iter;

	splitedMessage = split(std::string(originalHeader_.begin(), originalHeader_.end()), CRLF);
	parseHeader(splitedMessage);

	// for (iter = headers_.begin(); iter != headers_.end(); iter++)
	// 	std::cout << iter->first << " | " << iter->second << std::endl;
}

void Request::setFilePath() {
	filePath_ = locationBlock_.getWebRoot() + uri_->getOriginalUri();
}

void Request::setOriginalHeader(const std::vector<char> originalHeader) {
	originalHeader_ = originalHeader;
}

void	Request::setOriginalBody(const std::vector<char> originalBody) {
	originalBody_ = originalBody;
}

void Request::appendOriginalHeader(const char *buf, int length) {
	originalHeader_.insert(originalHeader_.end(), buf, buf + length);
}

void Request::appendOriginalBody(const char *buf, int length) {
	originalBody_.insert(originalBody_.end(), buf, buf + length);
}
