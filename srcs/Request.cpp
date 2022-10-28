#include "Request.hpp"

extern timeval start;

Request::Request()
	: uri_(nullptr), bodyLength_(0), isChunkSize_(FT_TRUE) {}

Request::~Request() {
	delete uri_;
}

void Request::appendBody(const std::vector<char> &vec) {
	body_.insert(body_.end(), vec.begin(), vec.end());
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
		body_ = std::vector<char>(originalBody_.begin(), originalBody_.begin() + getContentLengthNumber());
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
	filePath_ = uri_->getFilePath();
}

void Request::setOriginalHeader(const std::vector<char> originalHeader) {
	originalHeader_ = originalHeader;
}

void	Request::setOriginalBody(const std::vector<char> originalBody) {
	originalBody_ = originalBody;
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
	std::vector<char>			splitedBody;
	std::vector<char>::iterator it;
	std::vector<char>::iterator it2;
	std::string					chunkSizeString;
	std::size_t					chunkSize;

	// timestamp("parseChunkedBody start", start);
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
			appendBody(std::vector<char>(originalBody_.begin(), it));
		}
		originalBody_ = std::vector<char>(it + strlen(crlf), originalBody_.end());
		isChunkSize_ = !isChunkSize_; // toggle: chunk size or chunk data
		it = std::search(originalBody_.begin(), originalBody_.end(), crlf, crlf + strlen(crlf));
	}
	// timestamp("parseChunkedBody end", start);
}
