#include "Config.hpp"
#include "../macro.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

std::vector<std::string> Config::splitLines(const std::string &line, std::string delim) {
	std::vector<std::string>	tokens;
	std::size_t					pos = 0;
	std::size_t					end;

	while (true) {
		end = line.find_first_of(delim, pos);
		if (end == std::string::npos) {
			break;
		}
		tokens.push_back(line.substr(pos, end - pos));
		pos = line.find_first_not_of(delim, end);
	}
	return tokens;
}

std::vector<std::string>	Config::readAndSplit(std::string filePathStr) {
	const char					*filePath = filePathStr.c_str();
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
	tokens = splitLines(line, std::string(" \n\t"));
	close(fd);
	return tokens;
}

void	Config::parseConfigFile(char *filePath) {
	std::vector<std::string>	tokens;
	int							tokenSize;

	tokens = readAndSplit(filePath);
	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); it++)
		std::cout << *it << std::endl;
	tokenSize = tokens.size();
	for (int i = 0; i < tokenSize; i++) {
		if (tokens[i] == "server") {
			if (tokens[++i] != "{")
				throw std::runtime_error("Wrong formatted configuration file.\n");
			ServerBlock	tmpServerBlock;
			tmpServerBlock.parseServerBlock(tokens, i);
			serverBlocks_.push_back(tmpServerBlock);
		} else
			throw std::runtime_error("Wrong directive type in configuration file.\n");
	}
}

Config::Config() {}

Config::Config(char *filePath) {
	parseConfigFile(filePath);
}

Config::~Config() {}

std::vector<ServerBlock> Config::getServerBlocks() {
	return serverBlocks_;
}
