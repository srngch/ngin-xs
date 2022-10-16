#include "Block.hpp"

ft_bool	Block::check_validation(std::vector<std::string> &tokens, int &i, std::string &directive) {
	directivesMap::iterator	it;

	// ";" 뒤에 오는 토큰이 없이 파일의 끝이면 에러
	if (i + 1 == static_cast<int>(tokens.size()))
		throw std::runtime_error("Invalid configuration file: semi-colon should not be the end of the file.\n");
	// ";" 뒤에 오는 토큰이 "}" 이면 유효
	if (!strcmp(tokens[i + 1].c_str(), "}"))
		return FT_TRUE;
	// ";" 뒤에 오는 토큰이 location 이면 유효
	if (!strcmp(tokens[i + 1].c_str(), "location"))
		return FT_TRUE;
	// ";" 뒤에 오는 토큰이 지시자가 아니면 유효하지 않음
	it = directivesMap_.find(tokens[i + 1]);
	if (it == directivesMap_.end())
		throw std::runtime_error("Invalid configuration file: directive or } should exist after semi-colon.\n");
	// directive 가 저장되어 있지 않으면 에러
	if (directive == "")
		throw std::runtime_error("Invalid configuration file: directive should exist before semi-colon.\n");
	return FT_TRUE;
}

std::vector<std::string> Block::parseHostPort(const std::string &arg) {
	size_t						pos;
	std::vector<std::string>	args;

	pos = arg.find(":");
	// x.x.x.x:80 과 같은 형태가 아니면 에러
	if (pos == std::string::npos || pos == 0 || pos == arg.length() - 1)
		throw std::runtime_error("Wrong formatted configuration file: Listen x.x.x.x:port\n");
	// host(x.x.x.x)
	args.push_back(arg.substr(0, pos));
	// port(80)
	args.push_back(arg.substr(pos + 1, arg.length() - 1 - pos));

	return args;
}

Block::Block() : parent_(nullptr), host_(""), port_(0), webRoot_(""), clientMaxBodySize_(0), uri_(""), index_(""), autoIndex_("") {
	setServerDirectivesMap();
}

Block::Block(Block *parent) : host_(""), port_(0), webRoot_(""), clientMaxBodySize_(0), uri_(""), index_(""), autoIndex_("") {
	parent_ = parent;
	setLocationDirectivesMap();
}

Block &Block::operator=(const Block &origin) {
	parent_ = origin.getParent();
	locationBlocks_ = origin.getLocationBlocks();
	directivesMap_ = getDirectivesMap();
	host_ = origin.getHost();
	port_ = origin.getPort();
	serverNames_ = origin.getServerNames();
	webRoot_ = origin.getWebRoot();
	allowedMethods_ = origin.getAllowedMethods();
	clientMaxBodySize_ = origin.getClientMaxBodySize();
	errorPages_ = origin.getErrorPages();
	uri_ = origin.getUri();
	index_ = origin.getIndex();
	autoIndex_ = origin.getAutoIndex();
	cgi_ = origin.getCgi();
	return (*this);
}

void Block::setServerDirectivesMap() {
	directivesMap_["listen"] = &Block::setHostPort;
	directivesMap_["server_name"] = &Block::setServerNames;
	directivesMap_["root"] = &Block::setWebRoot;
	directivesMap_["client_max_body_size"] = &Block::setClientMaxBodySize;
	directivesMap_["error_page"] = &Block::setErrorPages;
}

void Block::setLocationDirectivesMap() {
	directivesMap_["index"] = &Block::setIndex;
	directivesMap_["autoindex"] = &Block::setAutoIndex;
	directivesMap_["cgi"] = &Block::setCgi;
	directivesMap_["root"] = &Block::setWebRoot;
	directivesMap_["limit_except"] = &Block::setAllowedMethods;
	directivesMap_["error_page"] = &Block::setErrorPages;
}

directivesMap Block::getDirectivesMap() {
	return directivesMap_;
}

ft_bool Block::has_semi_colon(std::vector<std::string> &tokens, int &i, std::vector<std::string> *args, std::string &directive) {
	int	last;

	last = tokens[i].length() - 1;
	// 토큰이 ";" 자체인 경우
	if (tokens[i] == ";")
		return (check_validation(tokens, i, directive));
	// 토큰의 오른쪽 끝이 ";"인 경우
	else if (tokens[i][last] == ';') {
		tokens[i].erase(last, last);
		args->push_back(tokens[i]);
		return (check_validation(tokens, i, directive));
	}
	// 토큰이 ";"를 포함하지 않음
	else
		return FT_FALSE;
}

void Block::parseServerBlock(std::vector<std::string> &tokens, int &i) {
	int							tokenSize;
	directivesMap::iterator		it;
	std::string					directive = "";
	std::vector<std::string>	args;

	if (tokens[i] != "{")
		throw std::runtime_error("Wrong formatted configuration file.\n");
	tokenSize = tokens.size();
	i++;
	while (i < tokenSize && tokens[i] != "}")
	{
		it = directivesMap_.find(tokens[i]);
		// 토큰이 지시자에 해당하지 않음
		if (it == directivesMap_.end()) {
			// 로케이션 블록 파싱
			if (tokens[i] == "location") {
				Block	tmpLocationBlock(this);
	
				tmpLocationBlock.parseLocationBlock(tokens, ++i);
				locationBlocks_.push_back(tmpLocationBlock);
			}
			else {
				// ; 을 포함하고 있으면 멤버변수에 값 저장
				if (has_semi_colon(tokens, i, &args, directive)) {
					(this->*Block::getDirectivesMap()[directive])(args);
					directive = "";
					args.clear();
				}
				// ; 을 포함하지 않은 토큰은 args 에 저장
				else
					args.push_back(tokens[i]);
			}
		}
		// 토큰이 지시자에 해당
		else
			directive = tokens[i];
		i++;
	}
}

void Block::parseLocationBlock(std::vector<std::string> &tokens, int &i) {
	int							tokenSize;
	directivesMap::iterator		it;
	std::string					directive = "";
	std::vector<std::string>	args;

	if (tokens[i] == "{")
		throw std::runtime_error("Wrong formatted configuration file.\n");
	setUri(tokens[i]);
	if (tokens[++i] != "{")
		throw std::runtime_error("Wrong formatted configuration file.\n");
	i++;
	tokenSize = tokens.size();
	while (i < tokenSize && tokens[i] != "}")
	{
		it = directivesMap_.find(tokens[i]);
		// 토큰이 지시자에 해당하지 않음
		if (it == directivesMap_.end()) {
			// 로케이션 블록 파싱
			if (tokens[i] == "location") {
				Block	tmpLocationBlock(this);
	
				tmpLocationBlock.parseLocationBlock(tokens, ++i);
				locationBlocks_.push_back(tmpLocationBlock);
			}
			else {
				// ; 을 포함하고 있으면 멤버변수에 값 저장
				if (has_semi_colon(tokens, i, &args, directive)) {
						(this->*directivesMap_[directive])(args);
						directive = "";
						args.clear();
				}
				// ; 을 포함하지 않은 토큰은 args 에 저장
				else
					args.push_back(tokens[i]);
			}
		}
		// 토큰이 지시자에 해당
		else
			directive = tokens[i];
		i++;
	}
}

void Block::setUri(std::string uri) {
	uri_ = uri;
}

void Block::setIndex(std::vector<std::string> args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("index\n");
	index_ = args[0];
}

void Block::setHostPort(std::vector<std::string> args) {
	std::vector<std::string>	hostport;

	if (args.size() != 1)
		throw InvalidConfigFileException("host and port\n");
	hostport = parseHostPort(args[0]);
	host_ = hostport[0];
	port_ = atoi(hostport[1].c_str());
}

void Block::setServerNames(std::vector<std::string> args) {
	std::vector<std::string>::iterator	it;

	if (args.empty())
		throw InvalidConfigFileException("server name\n");
	for (it = args.begin(); it != args.end(); it++)
		serverNames_.insert(*it);
}

void Block::setWebRoot(std::vector<std::string> args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("invalid web root\n");
	webRoot_ = args[0];
}

void Block::setCgi(std::vector<std::string> args) {
	std::vector<std::string>::iterator	it;

	if (args.empty())
		throw InvalidConfigFileException("cgi\n");
	for (it = args.begin(); it != args.end(); it++)
		cgi_.insert(*it);
}

void Block::setAllowedMethods(std::vector<std::string> args) {
	std::vector<std::string>::iterator	it;

	if (args.empty())
		throw InvalidConfigFileException("invalid allowed methods\n");
	for (it = args.begin(); it != args.end(); it++)
		allowedMethods_.insert(*it);
}

void Block::setClientMaxBodySize(std::vector<std::string> args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("invalid client max body size\n");
	clientMaxBodySize_= atoi(args[0].c_str());
}

void Block::setErrorPages(std::vector<std::string> args) {
	if (args.size() != 2)
		throw InvalidConfigFileException("invalid error pages\n");
	errorPages_.insert(std::pair<int, std::string>(atoi(args[0].c_str()), args[1]));
}

void Block::setAutoIndex(std::vector<std::string> args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("invalid auto index\n");
	autoIndex_ = args[0];
}

Block *Block::getParent() const {
	return parent_;
}

const std::vector<Block> &Block::getLocationBlocks() const {
	return locationBlocks_;
}

const std::string &Block::getHost() const {
	if (host_ != "")
		return host_;
	else if (host_ == "" && parent_)
		return (parent_->getHost());
	throw InvalidConfigFileException("no port set\n");
}

const int &Block::getPort() const {
	if (port_)
		return port_;
	else if (port_ == 0 && parent_)
		return (parent_->getPort());
	throw InvalidConfigFileException("no port set\n");
}

const std::set<std::string> &Block::getServerNames() const {
	if (!serverNames_.empty())
		return serverNames_;
	else if (serverNames_.empty() && parent_)
		return (parent_->getServerNames());
	throw InvalidConfigFileException("no server names set\n");
}

const std::string &Block::getWebRoot() const {
	if (webRoot_ != "")
		return webRoot_;
	else if (webRoot_ == "" && parent_)
		return (parent_->getWebRoot());
	throw InvalidConfigFileException("no web root set\n");
}

const std::set<std::string> &Block::getAllowedMethods() const {
	if (!allowedMethods_.empty())
		return allowedMethods_;
	else if (allowedMethods_.empty() && parent_)
		return (parent_->getAllowedMethods());
	throw InvalidConfigFileException("no allowed methods set\n");
}

const int &Block::getClientMaxBodySize() const {
	if (clientMaxBodySize_)
		return clientMaxBodySize_;
	else if (clientMaxBodySize_  == 0 && parent_)
		return (parent_->getClientMaxBodySize());
	throw InvalidConfigFileException("no client body size set\n");
}

const std::map<int, std::string> &Block::getErrorPages() const{
	if (!errorPages_.empty())
		return (errorPages_);
	else if (errorPages_.empty() && parent_)
		return (parent_->getErrorPages());
	throw InvalidConfigFileException("no error pages set\n");
}

const std::string &Block::getErrorPage(int num) const {
	std::map<int, std::string>				pages;
	std::map<int, std::string>::iterator	it;

	pages = getErrorPages();
	it = pages.find(num);
	if (it == pages.end())
		throw InvalidConfigFileException("no error page set.");
	return (it->second);
}

const std::string &Block::getUri() const {
	if (uri_ != "")
		return (uri_);
	else if (uri_ == "" && parent_)
		return (parent_->getUri());
	throw InvalidConfigFileException("no uri set\n");
}

const std::string &Block::getIndex() const {
	if (index_ != "")
		return (index_);
	else if (index_ == "" && parent_)
		return (parent_->getIndex());
	throw InvalidConfigFileException("no index set\n");
}

const std::string &Block::getAutoIndex() const {
	if (autoIndex_ != "")
		return (autoIndex_);
	else if (autoIndex_ == "" && parent_)
		return (parent_->getAutoIndex());
	throw InvalidConfigFileException("no autoindex set\n");
}

const std::set<std::string> &Block::getCgi() const {
	if (!cgi_.empty())
		return (cgi_);
	else if (cgi_.empty() && parent_)
		return (parent_->getCgi());
	throw InvalidConfigFileException("no cgi set\n");
}

const Block &Block::getLocationBlock(std::string uri) {
	std::vector<Block>::iterator	it;

	for (it = locationBlocks_.begin(); it != locationBlocks_.end(); it++) {
		if (uri_ == uri)
			return *this;
		else
			return (it->getLocationBlock(uri));
	}
	throw InvalidLocationBlockException();
}

void Block::printBlock() {
	std::set<std::string>::iterator			itset;
	std::map<int, std::string>::iterator	itmap;

	std::cout << "Host: " << host_ << std::endl;
	std::cout << "Port: " << port_ << std::endl;
	std::cout << "Server names: ";
	for (itset = serverNames_.begin(); itset != serverNames_.end(); itset++)
		std::cout << *itset << " ";
	std::cout << std::endl;
	std::cout << "Web root: ";
	std::cout << webRoot_ << std::endl;
	std::cout << "Client max body size: ";
	std::cout << clientMaxBodySize_ << std::endl;
	std::cout << "Error pages: " << std::endl;
	for (itmap = errorPages_.begin(); itmap != errorPages_.end(); itmap++)
		std::cout << itmap->first << " " << itmap->second << std::endl;
	std::cout << "Index: ";
	std::cout << index_ << std::endl;
	std::cout << "Cgi: " << std::endl;
	for (itset = cgi_.begin(); itset != cgi_.end(); itset++)
		std::cout << *itset << " ";
	std::cout << std::endl;
	std::cout << "Auto index: ";
	std::cout << autoIndex_ << std::endl;
	std::cout << "Allowed methods: " << std::endl;
	for (itset = allowedMethods_.begin(); itset != allowedMethods_.end(); itset++)
		std::cout << *itset << " ";
	std::cout << std::endl;
}

Block::InvalidConfigFileException::InvalidConfigFileException(const std::string msg) {
	message_ = "Invalid configuration file: ";
	message_ += msg;
}

const char *Block::InvalidConfigFileException::what() const throw() {
	return message_.c_str();
}

Block::InvalidConfigFileException::~InvalidConfigFileException() throw() {}

const char *Block::InvalidLocationBlockException::what() const throw() {
	return "There is no location block with the requested uri.\n";
}
