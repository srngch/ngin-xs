#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <exception>
#include "macro.hpp"
#include "utils.hpp"
#include "Uri.hpp"
#include "config/Block.hpp"

class Request {
private:
	Block								locationBlock_;
	std::string							method_;
	Uri									*uri_;
	std::string							version_;
	std::map<std::string, std::string>	headers_;
	std::vector<char>					body_;
	std::string							filePath_;
	std::vector<char>					originalHeader_;
	std::vector<char>					originalBody_;
	std::size_t							bodyLength_;
	ft_bool								isChunkSize_; // or chunk data

	void								appendBody(const std::vector<char>::iterator &startIt, const std::vector<char>::iterator &endIt);

	void	parseHeader(const std::vector<std::string> &splitedMessage);

public:
	Request();
	~Request();

	const Block									&getLocationBlock();
	const std::string							&getMethod();
	const Uri									*getUri();
	const std::string							&getVersion();
	const std::map<std::string, std::string>	&getHeaders();
	const std::string							getHeaderValue(const std::string &fieldName);
	const std::vector<char>						&getBody();
	const std::string							&getFilePath();
	const std::vector<char>						&getOriginalHeader();
	const std::vector<char>						&getOriginalBody();
	const std::size_t							&getBodyLength();

	std::size_t	getContentLengthNumber();
	
	void	setBody();
	void	setHeaders();
	void	setFilePath();
	void	setOriginalHeader(const std::vector<char>::iterator &startIt, const std::vector<char>::iterator &endIt);
	void	setOriginalBody(const std::vector<char>::iterator &startIt, const std::vector<char>::iterator &endIt);
	void	setLocationBlock(const Block &locationBlock);
	void	setBodyLength(const std::size_t bodyLength);

	void	appendOriginalHeader(const char *buf, int length);
	void	appendOriginalBody(const char *buf, int length);
	void	addBodyLength(const std::size_t length);
	void	parseChunkedBody();
};

#endif
