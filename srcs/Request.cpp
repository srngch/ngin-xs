#include "Request.hpp"

extern timeval start;

Request::Request()
	: uri_(nullptr), bodyLength_(0), isChunkSize_(FT_TRUE) {}

Request::~Request() {
	delete uri_;
}

void Request::appendBody(const vectorCharIter &startIt, const vectorCharIter &endIt) {
	body_.insert(body_.end(), startIt, endIt);
}

void Request::parseHeader(const vectorString &splitedMessage) {
	vectorStringConstIter	it = splitedMessage.begin();
	vectorString			splitedRequestLine = split(*it, " ");
	vectorString			splitedHeaderLine;

	// request line
	if (splitedRequestLine.size() != 3)
		throw std::runtime_error("parseHeader: request line parsing failed");
	method_ = splitedRequestLine[0];
	uri_ = new Uri(splitedRequestLine[1]);
	version_ = splitedRequestLine[2];
	it++;
	// headers
	while (it != splitedMessage.end()) {
		splitedHeaderLine = split(*it, ": ");
		std::transform(splitedHeaderLine[0].begin(), splitedHeaderLine[0].end(), splitedHeaderLine[0].begin(), ::tolower);
		headers_.insert(std::pair<std::string, std::string>(splitedHeaderLine[0], splitedHeaderLine[1]));
		it++;
	}
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

void Request::parseChunkedBody() {
	const char 		*crlf = "\r\n";
	vectorCharIter	it;
	std::string		chunkSizeString;
	std::size_t		chunkSize;

	it = std::search(originalBody_.begin(), originalBody_.end(), crlf, crlf + strlen(crlf));
	while (it != originalBody_.end()) {
		if (isChunkSize_) {
			chunkSizeString = std::string(originalBody_.begin(), it);
			chunkSize = hexStringToNumber(chunkSizeString);
			if (chunkSize == 0)
				break;
			addBodyLength(chunkSize);
		} else {
			// is chunk data
			appendBody(originalBody_.begin(), it);
		}
		originalBody_.assign(it + strlen(crlf), originalBody_.end());
		isChunkSize_ = !isChunkSize_; // toggle: chunk size or chunk data
		it = std::search(originalBody_.begin(), originalBody_.end(), crlf, crlf + strlen(crlf));
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

const mapStringString &Request::getHeaders() {
	return headers_;
}

const std::string Request::getHeaderValue(const std::string &fieldName) {
	mapStringStringIter	it = headers_.find(fieldName);
	if (it == headers_.end())
		return std::string("");
	return it->second;
}

const vectorChar &Request::getBody() {
	return body_;
}

const std::string &Request::getFilePath() {
	return filePath_;
}

const vectorChar &Request::getOriginalHeader() {
	return originalHeader_;
}

const vectorChar &Request::getOriginalBody() {
	return originalBody_;
}

const std::size_t &Request::getBodyLength() {
	return bodyLength_;
}

std::size_t Request::getContentLengthNumber() {
	return atoi(getHeaderValue("content-length").c_str());
}

void Request::setBody() {
	if (getHeaderValue("transfer-encoding") != "chunked")
		body_.assign(originalBody_.begin(), originalBody_.begin() + getContentLengthNumber());
}

void Request::setHeaders() {
	vectorString	splitedMessage;

	splitedMessage = split(std::string(originalHeader_.begin(), originalHeader_.end()), CRLF);
	parseHeader(splitedMessage);
}

void Request::setFilePath() {
	filePath_ = uri_->getFilePath();
}

void Request::setOriginalHeader(const vectorCharIter &startIt, const vectorCharIter &endIt) {
	originalHeader_.assign(startIt, endIt);
}

void Request::setOriginalBody(const vectorCharIter &startIt, const vectorCharIter &endIt) {
	originalBody_.assign(startIt, endIt);
}

void Request::setLocationBlock(const Block &locationBlock) {
	locationBlock_ = locationBlock;
	uri_->parseUri(locationBlock_);
}

void Request::setBodyLength(const std::size_t bodyLength) {
	bodyLength_ = bodyLength;
}
