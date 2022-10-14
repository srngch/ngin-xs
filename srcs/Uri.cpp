#include "Uri.hpp"

Uri::Uri() {}

Uri::Uri(const std::string &originalUri) {
	/*
	** not CGI
  ** /dir/subdir/file.html?qs2=1&qs2=2
	**
	** CGI
  ** /dir/subdir/file.py/path/info?qs2=1&qs2=2
  */
	originalUri_ = originalUri;
	
	std::string cgiExtension = ".py"; // TODO: get from Config class
	ft_bool isCgi = false;
	std::string	tmp;
	std::size_t	index;

	index = originalUri.find('?');
	queryString_ = "";
	if (index != std::string::npos)
		queryString_ = originalUri.substr(index);
	tmp = originalUri.substr(0, index);

	index = tmp.find(cgiExtension);
	/* 
	** CGI 확장자를 가짐 && tmp가 /로 끝나지 않음 (디렉토리가 아님)
	** tmp가 /로 끝난다면, URI를 디렉토리로 인식해야 됨
	** 예시: `/dir/cgi.bla/` -> 디렉토리
	*/
	if (index != std::string::npos && tmp.find('/', index) != std::string::npos) 
		isCgi = true;
	pathInfo_ = "";
	if (isCgi) {
		pathInfo_ = tmp.substr(index + cgiExtension.length() + 1);
		tmp = tmp.substr(0, index + cgiExtension.length() + 1);
	}
	index = tmp.rfind('/');
	basename_ = "";
	if (index != tmp.length() - 1) {
		basename_ = tmp.substr(index + 1);
		tmp = tmp.substr(0, index + 1);
	}
	uriDir_ = tmp;
	dir_ = uriDir_;

	// std::cout << "[Uri Class]" << std::endl;
	// std::cout << "uriDir: " << uriDir_ << std::endl;
	// std::cout << "basename_: " << basename_ << std::endl;
	// std::cout << "pathInfo_: " << pathInfo_ << std::endl;
	// std::cout << "queryString_: " << queryString_ << std::endl << std::endl;
}

Uri::~Uri() {}

const std::string &Uri::getOriginalUri() const {
	return originalUri_;
}

const std::string &Uri::getDir() const {
	return dir_;
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

// std::string Uri::getPath() {
// 	return dir_ + basename_;
// }
