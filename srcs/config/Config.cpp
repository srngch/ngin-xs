#include "Config.hpp"

Config::Config() {
	// Block::setDefaultBlock(DEFAULT_CONF_FILE_PATH);
}

Config::Config(const Config &origin) {
	*this = origin; 
}

Config::~Config() {}

Config &Config::operator=(const Config &origin) {
	if (this != &origin) {
		serverBlocks_ = origin.serverBlocks_;
	}
	return (*this);
}

vectorString Config::readAndSplit(const char *filePath) {
	char			buffer[CONFIG_BUF_SIZE + 1];
	int				fd;
	int				ret;
	std::string		line = "";
	vectorString	tokens;

	memset(buffer, 0, sizeof(buffer));
	fd = open(filePath, O_RDONLY);
	if (fd <= 0)
		throw std::runtime_error("readAndSplit: Configuration file opening failed");
	ret = read(fd, buffer, CONFIG_BUF_SIZE);
	if (ret == 0) {
		serverBlocks_.push_back(Block::defaultBlock_);
		return tokens;
	}
	while (ret > 0) {
		buffer[ret] = '\0';
		line += buffer;
		ret = read(fd, buffer, CONFIG_BUF_SIZE);
	}
	if (ret == FT_ERROR) {
		close(fd);
		throw std::runtime_error("readAndSplit: Configuration file reading failed");
	}
	tokens = parseLine(line, std::string(" \n\t"));
	close(fd);
	return tokens;
}

void Config::setHostPorts() {
	std::vector<Block>::iterator	it;

	for (it = serverBlocks_.begin(); it != serverBlocks_.end(); it++)
		hostPorts_.insert(it->getHostPort());
}

void Config::parseConfigFile(const char *filePath) {
	vectorString	tokens;
	int				tokenSize;

	if (!filePath)
		filePath = DEFAULT_CONF_FILE_PATH;
	tokens = readAndSplit(filePath);
	tokenSize = tokens.size();

	for (int i = 0; i < tokenSize; i++) {
		if (tokens[i] == "server") {
			Block	tmpServerBlock;
			int		port;

			tmpServerBlock.setServerDirectivesMap();
			tmpServerBlock.parseServerBlock(tokens, ++i);
			port = tmpServerBlock.getPort();
			if (port == 80 || port >= 1024)
				serverBlocks_.push_back(tmpServerBlock);
		} else
			throw std::runtime_error("parseConfigFile: Wrong directive type in configuration file");
	}
	setHostPorts();
}

const std::vector<Block> &Config::getServerBlocks() const {
	return serverBlocks_;
}

const Block &Config::getServerBlock(int port, const std::string &serverName) {
	std::vector<Block>::iterator	it;
	Block							*ret;

	for (it = serverBlocks_.begin(); it != serverBlocks_.end(); it++) {
		if (it->getPort() == port) {
			setString		serverNames = it->getServerNames();
			setStringIter	sIt;

			ret = &(*it);
			for (sIt = serverNames.begin(); sIt != serverNames.end(); sIt++) {
				if (*sIt == serverName)
					return *it;
			}
		}
	}
	return *ret;
}

const std::set<std::string> &Config::getHostPorts() const {
	return hostPorts_;
}
