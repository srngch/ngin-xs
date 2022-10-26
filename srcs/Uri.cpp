#include "Uri.hpp"

Uri::Uri() {}

Uri::Uri(const std::string &originalUri) 
: originalUri_(originalUri){
	parseQueryString();
}

Uri::~Uri() {}

void Uri::parseQueryString() {
	/*
	** Separate query string from requested original uri
	**
  ** /dir/subdir/file.html?qs2=1&qs2=2
	** -> 
	** 1. uriDir_: /dir/subdir/file.html
	** 2. queryString_: ?qs2=1&qs2=2
  */
	std::size_t	index;

	index = originalUri_.find('?');
	queryString_ = "";
	if (index != std::string::npos)
		queryString_ = originalUri_.substr(index);
	uriDir_ = originalUri_.substr(0, index);
}

void	Uri::parsePathInfo(const std::set<std::string> &supportedExtensions) {
	// Separate path_info with cgi extension setted in conf file
	
	/*
	** not CGI
  ** /dir/subdir/file.html?qs2=1&qs2=2
	**
	** CGI
  ** /dir/subdir/file.py/path/info?qs2=1&qs2=2
  */
	std::size_t						index;
	std::string						cgiExtension;
	std::set<std::string>::iterator	it;
	ft_bool							isCgi = FT_FALSE;

	for (it = supportedExtensions.begin(); it != supportedExtensions.end(); it++) {
		index = uriDir_.find("." + *it);
		if (index != std::string::npos) {
			cgiExtension = "." + *it;
			break;
		}
	}
	/* 
	** CGI 확장자를 가짐 && uriDir_가 /로 끝나지 않음 (디렉토리가 아님)
	** uriDir_가 /로 끝난다면, URI를 디렉토리로 인식해야 됨
	** 예시: `/dir/cgi.bla/` -> 디렉토리
	*/
	if (index != std::string::npos && uriDir_.find('/', index) != std::string::npos) 
		isCgi = FT_TRUE;
	if (isCgi) {
		pathInfo_ = uriDir_.substr(index + cgiExtension.length() + 1);
		uriDir_ = uriDir_.substr(0, index + cgiExtension.length() + 1);
	}
}

void Uri::parseBasename(const std::string &locationUri, const std::string &webroot) {
	std::size_t	index;
	std::string	parsedLocationUri;
	std::string	parsedUri;

	index = uriDir_.rfind('/');
	if (index != uriDir_.length() - 1) {
		basename_ = uriDir_.substr(index + 1);
		uriDir_ = uriDir_.substr(0, index + 1);
	}
	index = uriDir_.find_last_of("/");
	if (index != std::string::npos && index != uriDir_.length() - 1) 
		uriDir_ += "/";
	// uriDir과 location 블록을 비교해서, 서버 파일 시스템의 실제 dir 로 변경해주기
	parsedLocationUri = locationUri.substr(0, locationUri.find_last_of("/"));
	parsedUri = uriDir_;
	if (getParsedUri() == parsedLocationUri)
		parsedUri = getParsedUri();
	std::cout << "parsedLocationUri: " << parsedLocationUri << std::endl;
	std::cout << "parsedUri: " << parsedUri << std::endl;
	index = parsedUri.find(parsedLocationUri);
	if (parsedLocationUri == parsedUri)
		basename_ = "";
	parsedUri = parsedUri.substr(parsedLocationUri.length());
	if (index == 0) {
		if (parsedUri.find("/") == 0)
			pathDir_ = webroot + parsedUri;
		else
			pathDir_ = webroot + "/" + parsedUri;
	}
}

const std::string &Uri::getOriginalUri() const {
	return originalUri_;
}

const std::string &Uri::getPathDir() const {
	return pathDir_;
}

const std::string &Uri::getBaseName() const {
	return basename_;
}

const std::string &Uri::getPathInfo() const {
	return pathInfo_;
}

const std::string &Uri::getQueryString() const{
	return queryString_;
}

std::string Uri::getFilePath() const {
	return pathDir_ + basename_;
}

std::string Uri::getParsedUri() const {
	return uriDir_ + basename_;
}

void	Uri::parseUri(const Block &locationBlock) {
	parsePathInfo(locationBlock.getSupportedExtensions());
	parseBasename(locationBlock.getUri(), locationBlock.getWebRoot());

	std::cout << "locationBlock.getUri(): " << locationBlock.getUri() << std::endl;
	std::cout << "[Uri Class]" << std::endl;
	std::cout << "OriginalUri: " << originalUri_ << std::endl;
	std::cout << "uriDir: " << uriDir_ << std::endl;
	std::cout << "pathDir_: " << pathDir_ << std::endl;
	std::cout << "basename_: " << basename_ << std::endl;
	std::cout << "pathInfo_: " << pathInfo_ << std::endl;
	std::cout << "queryString_: " << queryString_ << std::endl << std::endl;
}
