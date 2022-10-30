#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "../types.hpp"
#include "Block.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <vector>

class Config {
private:
	std::vector<Block>	serverBlocks_;

	vectorString	readAndSplit(const char *filePath);

public:
	Config();
	Config(const Config &origin);
	~Config();
	Config &operator=(const Config &origin);

	void						parseConfigFile(const char *filePath);

	const std::vector<Block>	&getServerBlocks() const;
	const Block					&getServerBlock(int port) const;
};

#endif
