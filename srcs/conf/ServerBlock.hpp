#ifndef __SERVERBLOCK_HPP__
# define __SERVERBLOCK_HPP__

# include <set>
# include <string>
# include <map>
# include "LocationBlock.hpp"

class LocationBlock;

class ServerBlock {
private:
	// int							listenPort_;
	// int							clientBodyMaxSize_;
	// std::set<std::string>		serverNames_;
	// std::string					root_;
	// std::set<std::string>		cgi_;
	// std::set<std::string>		allowedMethods_;
	// std::map<int, std::string>	errorPages_;
	// std::vector<LocationBlock>	locations_;

public:
	ServerBlock();
	~ServerBlock();
};

#endif
