#include "Config.hpp"

std::vector<std::string> Config::readAndSplit(const char *filePath) {
	char						buffer[BUFFER_SIZE + 1];
	int							fd;
	int							ret;
	std::string					line = "";
	std::vector<std::string>	tokens;

	memset(buffer, 0, sizeof(buffer));
	fd = open(filePath, O_RDONLY);
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
	return tokens;
}

void Config::parseConfigFile(const char *filePath) {
	std::vector<std::string>	tokens;
	int							tokenSize;

	tokens = readAndSplit(filePath);
	tokenSize = tokens.size();
	for (int i = 0; i < tokenSize; i++) {
		if (tokens[i] == "server") {
			Block	tmpServerBlock;

			tmpServerBlock.setServerDirectivesMap();
			tmpServerBlock.parseServerBlock(tokens, ++i);
			serverBlocks_.push_back(tmpServerBlock);
		} else
			throw std::runtime_error("Wrong directive type in configuration file.\n");
	}
}

const std::vector<Block> &Config::getServerBlocks() const {
	return serverBlocks_;
}

const Block &Config::getServerBlock(int port) const {
	std::vector<Block>::const_iterator	it;

	for (it = serverBlocks_.begin(); it != serverBlocks_.end(); it++) {
		if (it->getPort() == port)
			return (*it);
	}
	return (serverBlocks_[0]);
}

Config::Config() {
	Block::setDefaultBlock(DEFAULT_CONF_FILE_PATH);
}

Config &Config::operator=(const Config &origin) {
	if (this != &origin) {
		serverBlocks_ = origin.serverBlocks_;
	}
	return (*this);
}

Config::Config(const Config &origin) {
	*this = origin; 
}

Config::~Config() {}

const char *Config::InvalidLocationBlockException::what() const throw() {
	return "There is no location block with the requested uri.\n";
}
