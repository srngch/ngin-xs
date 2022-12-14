#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <unistd.h>
#include <fcntl.h>
#include "Block.hpp"

class Config {
private:
	std::vector<Block>			serverBlocks_;
	std::set<std::string>		hostPorts_;

	vectorString	readAndSplit(const char *filePath);

	void	setHostPorts();

public:
	Config();
	Config(const Config &origin);
	~Config();
	Config &operator=(const Config &origin);

	void	parseConfigFile(const char *filePath);

	const std::vector<Block>	&getServerBlocks() const;
	std::vector<Block *>		getServerBlocks(std::string hostPort);
	const std::set<std::string>	&getHostPorts() const;
};

#endif
