#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# define BUFFER_SIZE 1600

# include "ServerBlock.hpp"
# include <vector>

class Config {
private:
	std::vector<ServerBlock>	serverBlocks_;
	std::vector<std::string> 	splitLines(const std::string &str, std::string delim);
	std::vector<std::string>	readAndSplit(std::string filePathStr);
	void						parseConfigFile(char *filePath);

public:
	Config();
	Config(char *filePath);
	~Config();

	std::vector<ServerBlock>	getServerBlocks();
};

#endif
