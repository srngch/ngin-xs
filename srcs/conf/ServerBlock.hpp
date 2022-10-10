#ifndef __SERVERBLOCK_HPP__
# define __SERVERBLOCK_HPP__

# include <set>
# include <string>
# include <vector>
# include <map>
# include "../macro.hpp"
# include "LocationBlock.hpp"

#define directivesMap std::map<std::string, void (ServerBlock::*)(std::vector<std::string>)>

class LocationBlock;

class ServerBlock {
private:
	// TODO: host:port 파트 나누어 저장
	std::string					listenPort_;
	std::set<std::string>		serverNames_;
	std::string					webRoot_;
	std::set<std::string>		cgi_;
	std::set<std::string>		allowedMethods_;
	std::map<int, std::string>	errorPages_;
	int							clientBodyMaxSize_;
	std::vector<LocationBlock>	locations_;

public:
	ServerBlock();
	~ServerBlock();
	
	directivesMap				getDirectivesMap();
	void						parseServerBlock(std::vector<std::string> &tokens, int &index);
	ft_bool						has_semi_colon(std::vector<std::string> &tokens, int &index);

	void						setListenPort(std::vector<std::string> args);
	void						setServerName(std::vector<std::string> args);
	void						setWebRoot(std::vector<std::string> args);
	void						setCgi(std::vector<std::string> args);
	void						setAllowedMethods(std::vector<std::string> args);
	void						setClientBodySize(std::vector<std::string> args);
};

#endif
