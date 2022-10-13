#include "Config.hpp"
#include "../macro.hpp"
#include "utils.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

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

			tmpServerBlock.parseServerBlock(tokens, ++i);
			// tmpServerBlock.printBlock();
			serverBlocks_.push_back(tmpServerBlock);
		} else
			throw std::runtime_error("Wrong directive type in configuration file.\n");
	}
}

std::vector<Block> Config::getServerBlocks() {
	return serverBlocks_;
}

// Block &Config::getServerBlock(int port) {}

// Block &Config::getLocationBlock(std::string uri) {}

Config::Config() {}

Config::Config(char *filePath) {
	parseConfigFile(filePath);
}

Config::~Config() {}
