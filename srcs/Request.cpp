#include "Request.hpp"

extern timeval start;

Request::Request()
	: uri_(nullptr), bodyLength_(0), isChunkSize_(FT_TRUE) {}

Request::~Request() {
	delete uri_;
}

void Request::appendBody(const std::vector<char>::iterator &startIt, const std::vector<char>::iterator &endIt) {
	body_.insert(body_.end(), startIt, endIt);
}

void	Request::parseHeader(const std::vector<std::string> &splitedMessage) {
	std::vector<std::string>::const_iterator	it = splitedMessage.begin();
	std::vector<std::string> splitedRequstLine = split(*it, " ");
	std::vector<std::string> splitedHeaderLine;

	// request line
	if (splitedRequstLine.size() != 3)
		throw std::runtime_error("parse error");
	method_ = splitedRequstLine[0];
	uri_ = new Uri(splitedRequstLine[1]);
	version_ = splitedRequstLine[2];
	it++;
	// headers
	while (it != splitedMessage.end()) {
		splitedHeaderLine = split(*it, ": ");
		std::transform(splitedHeaderLine[0].begin(), splitedHeaderLine[0].end(), splitedHeaderLine[0].begin(), ::tolower);
		headers_.insert(std::pair<std::string, std::string>(splitedHeaderLine[0], splitedHeaderLine[1]));
		it++;
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

const std::string Request::getHeaderValue(const std::string &fieldName) {
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

std::size_t Request::getContentLengthNumber() {
	return atoi(getHeaderValue("content-length").c_str());
}

const std::size_t &Request::getBodyLength() {
	return bodyLength_;
}

void Request::setLocationBlock(const Block &locationBlock) {
	locationBlock_ = locationBlock;
	uri_->parseUri(locationBlock_);
}

void	Request::setBody() {
	if (getHeaderValue("transfer-encoding") != "chunked")
		body_.assign(originalBody_.begin(), originalBody_.begin() + getContentLengthNumber());
}

void Request::setHeaders() {
	std::vector<std::string>	splitedMessage;

	splitedMessage = split(std::string(originalHeader_.begin(), originalHeader_.end()), CRLF);
	parseHeader(splitedMessage);

	// std::map<std::string, std::string>::iterator	iter;
	// for (iter = headers_.begin(); iter != headers_.end(); iter++)
	// 	std::cout << iter->first << " | " << iter->second << std::endl;
}

void Request::setFilePath() {
	filePath_ = uri_->getFilePath();
}

void Request::setOriginalHeader(const std::vector<char>::iterator &startIt, const std::vector<char>::iterator &endIt) {
	originalHeader_.assign(startIt, endIt);
}

void	Request::setOriginalBody(const std::vector<char>::iterator &startIt, const std::vector<char>::iterator &endIt) {
	originalBody_.assign(startIt, endIt);
}

void Request::setBodyLength(const std::size_t bodyLength) {
	bodyLength_ = bodyLength;
}

void Request::appendOriginalHeader(const char *buf, int length) {
	originalHeader_.insert(originalHeader_.end(), buf, buf + length);
}

void Request::appendOriginalBody(const char *buf, int length) {
	originalBody_.insert(originalBody_.end(), buf, buf + length);
}

void Request::addBodyLength(const std::size_t length) {
	bodyLength_ += length;
}

void	Request::parseChunkedBody() {
	const char 					*crlf = "\r\n";
	std::vector<char>::iterator it;
	std::string					chunkSizeString;
	std::size_t					chunkSize;

	// timestamp("* parseChunkedBody start", start);
	it = std::search(originalBody_.begin(), originalBody_.end(), crlf, crlf + strlen(crlf));
	while (it != originalBody_.end()) {
		if (isChunkSize_) {
			chunkSizeString = std::string(originalBody_.begin(), it);
			chunkSize = hexStringToNumber(chunkSizeString);
			if (chunkSize == 0) {
				break;
			}
			addBodyLength(chunkSize);
		} else {
			// is chunk data
			appendBody(originalBody_.begin(), it);
		}
		originalBody_.assign(it + strlen(crlf), originalBody_.end());
		isChunkSize_ = !isChunkSize_; // toggle: chunk size or chunk data
		it = std::search(originalBody_.begin(), originalBody_.end(), crlf, crlf + strlen(crlf));
	}
	// timestamp("* parseChunkedBody end", start);
}
