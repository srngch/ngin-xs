#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# include "Block.hpp"
# include <unistd.h>
# include <fcntl.h>
# include <vector>

class Config {
private:
	std::vector<Block>			serverBlocks_;

	std::vector<std::string>	readAndSplit(const char *filePath);

public:
	Config();
	Config(const Config &origin);
	~Config();
	Config &operator=(const Config &origin);

	void						parseConfigFile(const char *filePath);

	const std::vector<Block>	&getServerBlocks() const;
	const Block					&getServerBlock(int port) const;

	class InvalidLocationBlockException : public std::exception {
		virtual const char *what() const throw();
	};
};

#endif
