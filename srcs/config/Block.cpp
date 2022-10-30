#include "Block.hpp"
#include "Config.hpp"

Block Block::defaultBlock_;

ft_bool Block::checkParentUri(std::string uri) {
	int			len;

	/* parent의 uri */
	if (uri_ == "")
		return FT_TRUE;
	len = uri_.length();
	if (!uri.compare(0, len, uri_))
		return FT_TRUE;
	return FT_FALSE;
}

ft_bool	Block::checkValidation(vectorString &tokens, int &i, std::string &directive) {
	directivesMap::iterator	it;

	/* ";" 뒤에 오는 토큰이 없이 파일의 끝이면 에러 */
	if (i + 1 == static_cast<int>(tokens.size()))
		throw std::runtime_error("checkValidation: Semi-colon should not be the end of the file");
	/* ";" 뒤에 오는 토큰이 "}" 이면 유효 */
	if (!strcmp(tokens[i + 1].c_str(), "}"))
		return FT_TRUE;
	/* ";" 뒤에 오는 토큰이 location 이면 유효 */
	if (!strcmp(tokens[i + 1].c_str(), "location"))
		return FT_TRUE;
	/* ";" 뒤에 오는 토큰이 지시자가 아니면 유효하지 않음 */
	it = directivesMap_.find(tokens[i + 1]);
	if (it == directivesMap_.end())
		throw std::runtime_error("checkValidation: Directive or } should exist after semi-colon");
	/* directive 가 저장되어 있지 않으면 에러 */
	if (directive == "")
		throw std::runtime_error("checkValidation: Directive should exist before semi-colon");
	return FT_TRUE;
}

vectorString Block::parseHostPort(const std::string &arg) {
	size_t			pos;
	vectorString	args;

	pos = arg.find(":");
	/* x.x.x.x:80 과 같은 형태가 아니면 에러 */
	if (pos == std::string::npos || pos == 0 || pos == arg.length() - 1)
		throw std::runtime_error("parseHostPort: Wrong formatted configuration file (listen x.x.x.x:port)");
	/* host(x.x.x.x) */
	args.push_back(arg.substr(0, pos));
	/* port(80) */
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
	std::string			extension;
	setStringConstIter	it;

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

Block::Block() : host_(""), port_(0), webRoot_(""), clientMaxBodySize_(0), uri_(""), index_("index.html"), autoIndex_("off"), cgi_("") {
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
	vectorString	tokens;
	int				tokenSize;
	char			buffer[CONFIG_BUF_SIZE + 1];
	int				fd;
	int				ret;
	std::string		line = "";

	memset(buffer, 0, sizeof(buffer));
	fd = open(file, O_RDONLY);
	if (fd <= 0)
		throw std::runtime_error("setDefaultBlock: Configuration file opening failed");
	ret = read(fd, buffer, CONFIG_BUF_SIZE);
	while (ret > 0) {
		buffer[ret] = '\0';
		line += buffer;
		ret = read(fd, buffer, CONFIG_BUF_SIZE);
	}
	if (ret == FT_ERROR) {
		close(fd);
		throw std::runtime_error("setDefaultBlock: Configuration file reading failed");
	}
	tokens = parseLine(line, std::string(" \n\t"));
	close(fd);
	tokenSize = tokens.size();
	for (int i = 0; i < tokenSize; i++) {
		if (tokens[i] == "server") {
			Block	tmpServerBlock;

			tmpServerBlock.setServerDirectivesMap();
			tmpServerBlock.parseServerBlock(tokens, ++i);
			Block::defaultBlock_ = tmpServerBlock;
		} else
			throw std::runtime_error("setDefaultBlock: Wrong default configuration file");
	}
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

void Block::setChildLocationBlock(const Block &parent) {
	setLocationDirectivesMap();
	host_ = parent.host_;
	port_ = parent.port_;
	serverNames_ = parent.serverNames_;
	webRoot_ = parent.webRoot_;
	allowedMethods_ = parent.allowedMethods_;
	clientMaxBodySize_ = parent.clientMaxBodySize_;
	errorPages_ = parent.errorPages_;
	uri_ = parent.uri_;
	index_ = parent.index_;
	autoIndex_ = parent.autoIndex_;
	cgi_ = parent.cgi_;
}


directivesMap Block::getDirectivesMap() {
	return directivesMap_;
}

ft_bool Block::hasSemiColon(vectorString &tokens, int &i, vectorString *args, std::string &directive) {
	int	last;

	last = tokens[i].length() - 1;
	/* 토큰이 ";" 자체인 경우 */
	if (tokens[i] == ";")
		return (checkValidation(tokens, i, directive));
	/* 토큰의 오른쪽 끝이 ";"인 경우 */
	else if (tokens[i][last] == ';') {
		tokens[i].erase(last, last);
		args->push_back(tokens[i]);
		return (checkValidation(tokens, i, directive));
	}
	/* 토큰이 ";"를 포함하지 않음 */
	else
		return FT_FALSE;
}

void Block::parseServerBlock(vectorString &tokens, int &i) {
	int						tokenSize;
	directivesMap::iterator	it;
	std::string				directive = "";
	vectorString			args;
	ft_bool					hasSemiColonPrev = FT_TRUE;

	if (tokens[i] != "{")
		throw std::runtime_error("parseServerBlock: Wrong formatted configuration file");
	tokenSize = tokens.size();
	i++;
	while (i < tokenSize)
	{
		if (tokens[i] == "}" && validateSemiColon(hasSemiColonPrev))
			break ;
		it = directivesMap_.find(tokens[i]);
		/* 토큰이 지시자에 해당하지 않음 */
		if (it == directivesMap_.end()) {
			/* 로케이션 블록 파싱 */
			if (tokens[i] == "location") {
				Block	tmpLocationBlock;

				tmpLocationBlock.setChildLocationBlock(*this);
				validateSemiColon(hasSemiColonPrev);
				hasSemiColonPrev = FT_TRUE;
				tmpLocationBlock.setLocationDirectivesMap();
				tmpLocationBlock.parseLocationBlock(tokens, ++i);
				locationBlocks_.push_back(tmpLocationBlock);
			}
			else {
				/* ; 을 포함하고 있으면 멤버변수에 값 저장 */
				if (hasSemiColon(tokens, i, &args, directive)) {
					hasSemiColonPrev = FT_TRUE;
					(this->*Block::getDirectivesMap()[directive])(args);
					directive = "";
					args.clear();
				}
				/* ; 을 포함하지 않은 토큰은 args 에 저장 */
				else {
					args.push_back(tokens[i]);
					hasSemiColonPrev = FT_FALSE;
				}
			}
		}
		/* 토큰이 지시자에 해당 */
		else {
			validateSemiColon(hasSemiColonPrev);
			directive = tokens[i];
		}
		i++;
	}
	gatherSupportedExtensions();
}

ft_bool Block::validateSemiColon(ft_bool &hasSemiColonPrev) const {
	if (hasSemiColonPrev) {
		hasSemiColonPrev = FT_FALSE;
		return FT_TRUE;
	}
	else
		throw InvalidConfigFileException("validateSemiColon: No semi-colon");
}

void Block::parseLocationBlock(vectorString &tokens, int &i) {
	int						tokenSize;
	directivesMap::iterator	it;
	std::string				directive = "";
	vectorString			args;
	ft_bool					hasSemiColonPrev = FT_TRUE;

	if (tokens[i] == "{")
		throw std::runtime_error("parseLocationBlock: Wrong formatted configuration file");
	setUri(tokens[i]);
	addSupportedExtension(tokens[i]);
	if (tokens[++i] != "{")
		throw std::runtime_error("parseLocationBlock: Wrong formatted configuration file");
	i++;
	tokenSize = tokens.size();
	while (i < tokenSize)
	{
		if (tokens[i] == "}" && validateSemiColon(hasSemiColonPrev))
			break ;
		it = directivesMap_.find(tokens[i]);
		/* 토큰이 지시자에 해당하지 않음 */
		if (it == directivesMap_.end()) {
			/* 로케이션 블록 파싱 */
			if (tokens[i] == "location") {
				Block	tmpLocationBlock;

				tmpLocationBlock.setChildLocationBlock(*this);
				validateSemiColon(hasSemiColonPrev);
				hasSemiColonPrev = FT_TRUE;
				tmpLocationBlock.parseLocationBlock(tokens, ++i);
				locationBlocks_.push_back(tmpLocationBlock);
			}
			else {
				/* ; 을 포함하고 있으면 멤버변수에 값 저장 */
				if (hasSemiColon(tokens, i, &args, directive)) {
						hasSemiColonPrev = FT_TRUE;
						(this->*directivesMap_[directive])(args);
						directive = "";
						args.clear();
				}
				/* ; 을 포함하지 않은 토큰은 args 에 저장 */
				else {
					args.push_back(tokens[i]);
					hasSemiColonPrev = FT_FALSE;
				}
			}
		}
		/* 토큰이 지시자에 해당 */
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
		throw InvalidConfigFileException("setUri: Location block's uri doesn't contain parent's location uri");
	uri_ = uri;
}

void Block::setIndex(vectorString args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("setIndex: failed");
	index_ = args[0];
}

void Block::setHost(std::string host) {
	host_ = host;
}

void Block::setPort(int port) {
	port_ = port;
}

void Block::setHostPort(vectorString args) {
	vectorString	hostport;

	if (args.size() != 1)
		throw InvalidConfigFileException("setHostPort: failed");
	hostport = parseHostPort(args[0]);
	host_ = hostport[0];
	port_ = atoi(hostport[1].c_str());
}

void Block::setServerNames(vectorString args) {
	vectorStringIter	it;

	if (args.empty())
		throw InvalidConfigFileException("setServerNames: failed");
	for (it = args.begin(); it != args.end(); it++)
		serverNames_.insert(*it);
}

void Block::setWebRoot(vectorString args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("setWebRoot: failed");
	webRoot_ = args[0];
}

void Block::setCgi(vectorString args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("setCgi: falied");
	cgi_ = args[0];
}

void Block::setAllowedMethods(vectorString args) {
	vectorStringIter	it;

	if (args.empty())
		throw InvalidConfigFileException("setAllowedMethods: Invalid allowed methods");
	for (it = args.begin(); it != args.end(); it++)
		allowedMethods_.insert(*it);
}

void Block::setClientMaxBodySize(int size) {
	clientMaxBodySize_= size;
}

void Block::setClientMaxBodySize(vectorString args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("setClientMaxBodySize: Invalid client max body size");
	clientMaxBodySize_= atoi(args[0].c_str());
}

void Block::setErrorPages(vectorString args) {
	if (args.size() != 2)
		throw InvalidConfigFileException("setErrorPages: Invalid error pages");
	errorPages_.insert(std::pair<int, std::string>(atoi(args[0].c_str()), args[1]));
}

void Block::setAutoIndex(vectorString args) {
	if (args.size() != 1)
		throw InvalidConfigFileException("setAutoIndex: Invalid autoindex");
	autoIndex_ = args[0];
}

const setString &Block::getSupportedExtensions() const {
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

const setString &Block::getServerNames() const {
	if (!serverNames_.empty())
		return serverNames_;
	return Block::defaultBlock_.getServerNames();
}

const std::string &Block::getWebRoot() const {
	if (webRoot_ != "")
		return webRoot_;
	return Block::defaultBlock_.getWebRoot();
}

const setString &Block::getAllowedMethods() const {
	if (!allowedMethods_.empty())
		return allowedMethods_;
	return Block::defaultBlock_.getAllowedMethods();
}

const std::size_t &Block::getClientMaxBodySize() const {
	if (clientMaxBodySize_)
		return clientMaxBodySize_;
	return Block::defaultBlock_.getClientMaxBodySize();
}

const mapIntString &Block::getErrorPages() const {
	if (!errorPages_.empty())
		return (errorPages_);
	return Block::defaultBlock_.getErrorPages();
}

std::string Block::getErrorPage(int num) const {
	mapIntString		pages;
	mapIntStringIter	it;

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
	return index_;
}

ft_bool Block::getAutoIndex() const {
	if (autoIndex_ == "on")
		return FT_TRUE;
	else
		return FT_FALSE;
}

const std::string &Block::getCgi() const {
	return cgi_;
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
		if (ret.getUri() == "")
			continue;
		else
			return (it->getLocationBlockRecursive(uri));
	}
	return Block::defaultBlock_;
}

Block Block::getLocationBlock(std::string uri) const {
	int						dot;
	std::string				parsedUri;
	vectorString			uriVector;
	int						index;
	vectorStringReverseIter	rIt;
	Block					tmpBlock;
	Block					ret;
	std::string				tmpUri;

	/* 1. uri 가공 */
	if (isExtension(uri, dot)) {
		if (isCgiExtension(uri, dot))
			applyWildCard(uri, dot);
		else
			removeFileName(uri, dot);
	}
	/* 2. 슬래시(/) 단위로 uri 자르기 (예: /abc/def/ghi/ -> / /abc/ /abc/def/ /abc/def/ghi/) */
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
		tmpBlock = getLocationBlockRecursive(*rIt + "/");
		if (tmpBlock.getUri() != "")
			ret = tmpBlock;
	}
	return ret;
}

void Block::gatherSupportedExtensions() {
	std::vector<Block>::iterator	it;
	setString						supportedExtensions;
	setStringIter					setIt;

	for (it = locationBlocks_.begin(); it != locationBlocks_.end(); it++) {
		supportedExtensions = it->getSupportedExtensions();
		for (setIt = supportedExtensions.begin(); setIt != supportedExtensions.end(); setIt++) {
			supportedExtensions_.insert(*setIt);
		}
	}
}

void Block::printBlock() const {
	setStringIter		itset;
	mapIntString		pages;
	mapIntStringIter	itmap;

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
