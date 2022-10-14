#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# define BUFFER_SIZE 1600

# include "Block.hpp"
# include <vector>

class Config {
private:
	std::vector<Block>			serverBlocks_;

	std::vector<std::string> 	splitLines(const std::string &str, std::string delim);
	std::vector<std::string>	readAndSplit(const char *filePath);
	void						parseConfigFile(const char *filePath);

public:
	Config();
	Config(char *filePath);
	~Config();

	const std::vector<Block>	&getServerBlocks() const;
	// Block						&getServerBlock(int port);
	// Block						&getLocationBlock(std::string uri);
};

#endif
