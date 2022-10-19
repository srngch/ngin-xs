#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# define BUFFER_SIZE 1600

# include "Block.hpp"
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <vector>

class Config {
private:
	std::vector<Block>			serverBlocks_;

	Config();
	std::vector<std::string>	readAndSplit(const char *filePath);
	void						parseConfigFile(const char *filePath);

public:
	Config(char *filePath);
	~Config();

	const std::vector<Block>	&getServerBlocks();
	const Block					&getServerBlock(int port);
	const Block					&getLocationBlock(std::string uri);

	static Block				defaultBlock_;
};

#endif
