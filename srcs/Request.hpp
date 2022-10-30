#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <cctype>
#include "Uri.hpp"

class Request {
private:
	Block			locationBlock_;
	std::string		method_;
	Uri				*uri_;
	std::string		version_;
	mapStringString	headers_;
	vectorChar		body_;
	std::string		filePath_;
	vectorChar		originalHeader_;
	vectorChar		originalBody_;
	std::size_t		bodyLength_;
	ft_bool			isChunkSize_; // or chunk data

	void	appendBody(const vectorCharIter &startIt, const vectorCharIter &endIt);
	void	parseHeader(const vectorString &splitedMessage);

public:
	Request();
	~Request();

	void	appendOriginalHeader(const char *buf, int length);
	void	appendOriginalBody(const char *buf, int length);
	void	addBodyLength(const std::size_t length);
	void	parseChunkedBody();

	const Block				&getLocationBlock();
	const std::string		&getMethod();
	const Uri				*getUri();
	const std::string		&getVersion();
	const mapStringString	&getHeaders();
	const std::string		getHeaderValue(const std::string &fieldName);
	const vectorChar		&getBody();
	const std::string		&getFilePath();
	const vectorChar		&getOriginalHeader();
	const vectorChar		&getOriginalBody();
	const std::size_t		&getBodyLength();
	std::size_t				getContentLengthNumber();

	void	setBody();
	void	setHeaders();
	void	setFilePath();
	void	setOriginalHeader(const vectorCharIter &startIt, const vectorCharIter &endIt);
	void	setOriginalBody(const vectorCharIter &startIt, const vectorCharIter &endIt);
	void	setLocationBlock(const Block &locationBlock);
	void	setBodyLength(const std::size_t bodyLength);
};

#endif
