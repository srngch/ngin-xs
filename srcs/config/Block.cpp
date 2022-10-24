#include "Block.hpp"
#include "Config.hpp"

Block Block::defaultBlock_;

ft_bool Block::checkParentUri(std::string uri) {
	int			len;

	// parent의 uri
	if (uri_ == "")
		return FT_TRUE;
	len = uri_.length();
	if (!uri.compare(0, len, uri_))
	// if (!strncmp(uri.c_str(), uri_.c_str(), len))
		return FT_TRUE;
	return FT_FALSE;
}

ft_bool	Block::checkValidation(std::vector<std::string> &tokens, int &i, std::string &directive) {
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

ft_bool Block::isExtension(const std::string &uri, int &i) const {
	int		len;

	len = uri.length();
	i = uri.find_last_of('.');
	if (i <= 0 || i == len - 1)
		return FT_FALSE;
	return FT_TRUE;
}

ft_bool Block::isCgiExtension(const std::string &uri, int &i) const {
	std::string								extension;
	std::set<std::string>::const_iterator	it;

	extension = uri.substr(i + 1);
	for (it = supportedExtensions_.begin(); it != supportedExtensions_.end(); it++) {
		if (extension == *it)
			return FT_TRUE;
	}
	return FT_FALSE;
}

void Block::addSupportedExtension(const std::string &token) {
	int			len;
	int			i;
	std::string	extension;

	len = token.length();
	for (i = len; i >= 0; i--) {
		if (token[i] == '.') {
			if (i == len)
				return ;
			else
				break ;
		}
	}
	if (i <= 0)
		return ;
	if (token[i - 1] != '*')
		return ;
	extension = token.substr(i + 1);
	supportedExtensions_.insert(extension);
}

Block::Block() : host_(""), port_(0), webRoot_(""), clientMaxBodySize_(0), uri_(""), index_(""), autoIndex_("off") {
	setServerDirectivesMap();
}

Block::Block(const Block &origin) {
	*this = origin;
	if (origin.uri_ != "")
		setLocationDirectivesMap();
}

Block::~Block() {}

Block &Block::operator=(const Block &origin) {
	if (this != &origin) {
		locationBlocks_ = origin.locationBlocks_;
		directivesMap_ = origin.directivesMap_;
		supportedExtensions_ = origin.supportedExtensions_;
		host_ = origin.host_;
		port_ = origin.port_;
		serverNames_ = origin.serverNames_;
		webRoot_ = origin.webRoot_;
		allowedMethods_ = origin.allowedMethods_;
		clientMaxBodySize_ = origin.clientMaxBodySize_;
		errorPages_ = origin.errorPages_;
		uri_ = origin.uri_;
		index_ = origin.index_;
		autoIndex_ = origin.autoIndex_;
		cgi_ = origin.cgi_;
	}
	return (*this);
}

void Block::setDefaultBlock(const char *file) {
	std::vector<std::string>	tokens;
	int							tokenSize;
	char						buffer[BUFFER_SIZE + 1];
	int							fd;
	int							ret;
	std::string					line = "";

	memset(buffer, 0, sizeof(buffer));
	fd = open(file, O_RDONLY);
	if (fd <= 0)
		throw std::runtime_error("fail: Opening configuration file\n");
	ret = read(fd, buffer, BUFFER_SIZE);
	while (ret > 0) {
		buffer[ret] = '\0';
		line += buffer;
		ret = read(fd, buffer, BUFFER_SIZE);
	}
	if (ret == FT_ERROR) {
		close(fd);
		throw std::runtime_error("fail: Reading configuration file\n");
	}
	tokens = parseLine(line, std::string(" \n\t"));
	close(fd);
	tokenSize = tokens.size();
	for (int i = 0; i < tokenSize; i++) {
		if (tokens[i] == "server") {
			Block	tmpServerBlock;

			tmpServerBlock.setDefaultServerDirectivesMap();
			tmpServerBlock.parseServerBlock(tokens, ++i);
			Block::defaultBlock_ = tmpServerBlock;
		} else
			throw std::runtime_error("Wrong default configuration file.\n");
	}
}

void Block::setDefaultServerDirectivesMap() {
	directivesMap_["listen"] = &Block::setHostPort;
	directivesMap_["server_name"] = &Block::setServerNames;
	directivesMap_["root"] = &Block::setWebRoot;
	directivesMap_["allowed_methods"] = &Block::setAllowedMethods;
	directivesMap_["client_max_body_size"] = &Block::setClientMaxBodySize;
	directivesMap_["error_page"] = &Block::setErrorPages;
	directivesMap_["index"] = &Block::setIndex;
	directivesMap_["autoindex"] = &Block::setAutoIndex;
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
	directivesMap_["allowed_methods"] = &Block::setAllowedMethods;
	directivesMap_["error_page"] = &Block::setErrorPages;
}

directivesMap Block::getDirectivesMap() {
	return directivesMap_;
}

ft_bool Block::hasSemiColon(std::vector<std::string> &tokens, int &i, std::vector<std::string> *args, std::string &directive) {
	int	last;

	last = tokens[i].length() - 1;
	// 토큰이 ";" 자체인 경우
	if (tokens[i] == ";")
		return (checkValidation(tokens, i, directive));
	// 토큰의 오른쪽 끝이 ";"인 경우
	else if (tokens[i][last] == ';') {
		tokens[i].erase(last, last);
		args->push_back(tokens[i]);
		return (checkValidation(tokens, i, directive));
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
	ft_bool						hasSemiColonPrev = FT_TRUE;

	if (tokens[i] != "{")
		throw std::runtime_error("Wrong formatted configuration file.\n");
	tokenSize = tokens.size();
	i++;
	while (i < tokenSize)
	{
		if (tokens[i] == "}" && validateSemiColon(hasSemiColonPrev))
			break ;
		it = directivesMap_.find(tokens[i]);
		// 토큰이 지시자에 해당하지 않음
		if (it == directivesMap_.end()) {
			// 로케이션 블록 파싱
			if (tokens[i] == "location") {
				Block	tmpLocationBlock(*this);

				validateSemiColon(hasSemiColonPrev);
				hasSemiColonPrev = FT_TRUE;
				tmpLocationBlock.setLocationDirectivesMap();
				tmpLocationBlock.parseLocationBlock(tokens, ++i);
				locationBlocks_.push_back(tmpLocationBlock);
			}
			else {
				// ; 을 포함하고 있으면 멤버변수에 값 저장
				if (hasSemiColon(tokens, i, &args, directive)) {
					hasSemiColonPrev = FT_TRUE;
					(this->*Block::getDirectivesMap()[directive])(args);
					directive = "";
					args.clear();
				}
				// ; 을 포함하지 않은 토큰은 args 에 저장
				else {
					args.push_back(tokens[i]);
					hasSemiColonPrev = FT_FALSE;
				}
			}
		}
		// 토큰이 지시자에 해당
		else {
			validateSemiColon(hasSemiColonPrev);
			directive = tokens[i];
		}
		gatherSupportedExtensions();
		i++;
	}
}

ft_bool Block::validateSemiColon(ft_bool &hasSemiColonPrev) const {
	if (hasSemiColonPrev) {
		hasSemiColonPrev = FT_FALSE;
		return FT_TRUE;
	}
	else
		throw InvalidConfigFileException("parseLocationBlock: No semi-colon");
}

void Block::parseLocationBlock(std::vector<std::string> &tokens, int &i) {
	int							tokenSize;
	directivesMap::iterator		it;
	std::string					directive = "";
	std::vector<std::string>	args;
	ft_bool						hasSemiColonPrev = FT_TRUE;

	if (tokens[i] == "{")
		throw std::runtime_error("Wrong formatted configuration file.\n");
	setUri(tokens[i]);
	addSupportedExtension(tokens[i]);
	if (tokens[++i] != "{")
		throw std::runtime_error("Wrong formatted configuration file.\n");
	i++;
	tokenSize = tokens.size();
	while (i < tokenSize)
	{
		if (tokens[i] == "}" && validateSemiColon(hasSemiColonPrev))
			break ;
		it = directivesMap_.find(tokens[i]);
		// 토큰이 지시자에 해당하지 않음
		if (it == directivesMap_.end()) {
			// 로케이션 블록 파싱
			if (tokens[i] == "location") {
				Block	tmpLocationBlock(*this);

				validateSemiColon(hasSemiColonPrev);
				hasSemiColonPrev = FT_TRUE;
				tmpLocationBlock.parseLocationBlock(tokens, ++i);
				locationBlocks_.push_back(tmpLocationBlock);
			}
			else {
				// ; 을 포함하고 있으면 멤버변수에 값 저장
				if (hasSemiColon(tokens, i, &args, directive)) {
						hasSemiColonPrev = FT_TRUE;
						(this->*directivesMap_[directive])(args);
						directive = "";
						args.clear();
				}
				// ; 을 포함하지 않은 토큰은 args 에 저장
				else {
					args.push_back(tokens[i]);
					hasSemiColonPrev = FT_FALSE;
				}
			}
		}
		// 토큰이 지시자에 해당
		else {
			validateSemiColon(hasSemiColonPrev);
			directive = tokens[i];
		}
		gatherSupportedExtensions();
		i++;
	}
}

void Block::setUri(std::string uri) {
	if (!checkParentUri(uri))
		throw InvalidConfigFileException("error: location block's uri doesn't contain parent's location uri.\n");
	uri_ = uri;
}

void Block::setIndex(std::vector<std::string> args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("index\n");
	index_ = args[0];
}

void Block::setHost(std::string host) {
	host_ = host;
}

void Block::setPort(int port) {
	port_ = port;
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
	if (args.size() != 1)
		throw InvalidConfigFileException("cgi\n");
	cgi_ = args[0];
}

void Block::setAllowedMethods(std::vector<std::string> args) {
	std::vector<std::string>::iterator	it;

	if (args.empty())
		throw InvalidConfigFileException("invalid allowed methods\n");
	for (it = args.begin(); it != args.end(); it++)
		allowedMethods_.insert(*it);
}

void Block::setClientMaxBodySize(int size) {
	clientMaxBodySize_= size;
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

const std::set<std::string> &Block::getSupportedExtensions() const {
	return supportedExtensions_;
}

const std::vector<Block> &Block::getLocationBlocks() const {
	return locationBlocks_;
}

std::string Block::getHost() const {
	if (host_ == "localhost")
		return "0.0.0.0";
	if (host_ != "")
		return host_;
	return Block::defaultBlock_.getHost();
}

const int &Block::getPort() const {
	if (port_)
		return port_;
	return Block::defaultBlock_.getPort();
}

const std::set<std::string> &Block::getServerNames() const {
	if (!serverNames_.empty())
		return serverNames_;
	return Block::defaultBlock_.getServerNames();
}

const std::string &Block::getWebRoot() const {
	if (webRoot_ != "")
		return webRoot_;
	return Block::defaultBlock_.getWebRoot();
}

const std::set<std::string> &Block::getAllowedMethods() const {
	if (!allowedMethods_.empty())
		return allowedMethods_;
	return Block::defaultBlock_.getAllowedMethods();
}

const std::size_t &Block::getClientMaxBodySize() const {
	if (clientMaxBodySize_)
		return clientMaxBodySize_;
	return Block::defaultBlock_.getClientMaxBodySize();
}

const std::map<int, std::string> &Block::getErrorPages() const {
	if (!errorPages_.empty())
		return (errorPages_);
	return Block::defaultBlock_.getErrorPages();
}

std::string Block::getErrorPage(int num) const {
	std::map<int, std::string>				pages;
	std::map<int, std::string>::iterator	it;

	pages = getErrorPages();
	it = pages.find(num);
	if (it == pages.end())
		return "";
	return (it->second);
}

const std::string &Block::getUri() const {
	return uri_;
}

const std::string &Block::getIndex() const {
	if (index_ != "")
		return (index_);
	return Block::defaultBlock_.getIndex();
}

ft_bool Block::getAutoIndex() const {
	if (autoIndex_ == "on")
		return FT_TRUE;
	else if (autoIndex_ == "off")
		return FT_FALSE;
	return Block::defaultBlock_.getAutoIndex();
}

const std::string &Block::getCgi() const {
	if (cgi_ != "")
		return (cgi_);
	throw InvalidConfigFileException("no cgi set\n");
}

void Block::applyWildCard(std::string &uri, int &dot) const {
	int			slash = 0;

	slash = uri.find_last_of('/');
	if (slash == dot - 1)
		return ;
	uri.replace(slash + 1, dot - slash - 1, "*");
}

void Block::removeFileName(std::string &uri, int &dot) const {
	int			slash = 0;

	slash = uri.find_last_of('/');
	if (slash == dot - 1)
		return ;
	uri.erase(slash + 1);
}

const Block &Block::getLocationBlockRecursive(std::string uri) const {
	std::vector<Block>::const_iterator		it;
	Block									ret;

	if (uri_ == uri)
		return *this;
	if (locationBlocks_.empty())
		return Block::defaultBlock_;
	for (it = locationBlocks_.begin(); it != locationBlocks_.end(); it++) {
		ret = it->getLocationBlockRecursive(uri);
		if (ret.getUri() == "") {
			continue;
		}
		else {
			return (it->getLocationBlockRecursive(uri));
		}
	}
}

Block Block::getLocationBlock(std::string uri) const {
	int											dot;
	std::string									parsedUri;
	std::vector<std::string>					uriVector;
	int											index;
	std::vector<std::string>::reverse_iterator	rIt;
	Block										tmpBlock;
	Block										ret;

	// 1. uri 가공
	if (isExtension(uri, dot)) {
		if (isCgiExtension(uri, dot))
			applyWildCard(uri, dot);
		else
			removeFileName(uri, dot);
	}
	// 2. 슬래시(/) 단위로 uri 자르기 (예: /abc/def/ghi/ -> / /abc/ /abc/def/ /abc/def/ghi/)
	// /test/dir/
	parsedUri = uri;
	while (FT_TRUE) {
		uriVector.push_back(parsedUri);
		if (parsedUri == "/")
			break ;
		index = parsedUri.find_last_of("/", parsedUri.length() - 2);
		parsedUri = parsedUri.substr(0, index + 1);
	}

	for (rIt = uriVector.rbegin(); rIt != uriVector.rend(); rIt++) {
		tmpBlock = getLocationBlockRecursive(*rIt);
		if (tmpBlock.getUri() != "")
			ret = tmpBlock;
	}
	return ret;
}

void Block::gatherSupportedExtensions() {
	std::vector<Block>::iterator	it;
	std::set<std::string>			supportedExtensions;
	std::set<std::string>::iterator	setIt;

	for (it = locationBlocks_.begin(); it != locationBlocks_.end(); it++) {
		supportedExtensions = it->getSupportedExtensions();
		for (setIt = supportedExtensions.begin(); setIt != supportedExtensions.end(); setIt++)
			supportedExtensions_.insert(*setIt);
	}
}

void Block::printBlock() const {
	std::set<std::string>::iterator			itset;
	std::map<int, std::string>				pages;
	std::map<int, std::string>::iterator	itmap;

	std::cout << "Host: " << getHost() << std::endl;
	std::cout << "Port: " << getPort() << std::endl;
	std::cout << "Server names: ";
	for (itset = getServerNames().begin(); itset != getServerNames().end(); itset++)
		std::cout << *itset << " ";
	std::cout << std::endl;
	std::cout << "Web root: ";
	std::cout << getWebRoot() << std::endl;
	std::cout << "Client max body size: ";
	std::cout << getClientMaxBodySize() << std::endl;
	std::cout << "Error pages: " << std::endl;
	pages = getErrorPages();
	for (itmap = pages.begin(); itmap != pages.end(); itmap++)
		std::cout << itmap->first << " " << itmap->second << std::endl;
	std::cout << "Index: ";
	std::cout << getIndex() << std::endl;
	try {
		std::cout << "Cgi: ";
		std::cout << getCgi() << std::endl;
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	std::cout << "Auto index: ";
	std::cout << getAutoIndex() << std::endl;
	std::cout << "Allowed methods: " << std::endl;
	for (itset = getAllowedMethods().begin(); itset != getAllowedMethods().end(); itset++)
		std::cout << *itset << " ";
	std::cout << std::endl;
	std::cout << "Uri: ";
	std::cout << getUri() << std::endl;
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
