#ifndef __BLOCK_HPP__
# define __BLOCK_HPP__

# include <cstdlib> 
# include <iostream>
# include <set>
# include <string>
# include <vector>
# include <map>
# include "../macro.hpp"

#define directivesMap std::map<std::string, void (Block::*)(std::vector<std::string>)>

class Block {
private:
	Block						*parent_;
	std::vector<Block>			locationBlocks_;
	directivesMap				directivesMap_;

	//TODO: host:port 나누어 저장
	// std::string					host_;
	// std::string					port_;
	std::string					listenPort_;
	std::set<std::string>		serverNames_;
	std::string					webRoot_;
	std::set<std::string>		allowedMethods_;
	int							clientBodyMaxSize_;
	std::map<int, std::string>	errorPages_;
	std::string					uri_;
	std::string					index_;
	std::string					autoIndex_;
	std::set<std::string>		cgi_;

	ft_bool						check_validation(std::vector<std::string> &tokens, int &index, std::string &directive);

public:
	Block();
	Block(Block *parent);
	
	void						setServerDirectivesMap();
	void						setLocationDirectivesMap();
	directivesMap				getDirectivesMap();
	
	ft_bool						has_semi_colon(std::vector<std::string> &tokens, int &index, std::vector<std::string> *args, std::string &directive);
	void						parseServerBlock(std::vector<std::string> &tokens, int &index);
	void						parseLocationBlock(std::vector<std::string> &tokens, int &i);
	
	// ServerBlock 만 해당
	void						setListenPort(std::vector<std::string> args);
	void						setServerNames(std::vector<std::string> args);
	// ServerBlock, LocationBlock 공통
	void						setWebRoot(std::vector<std::string> args);
	void						setAllowedMethods(std::vector<std::string> args);
	void						setClientBodySize(std::vector<std::string> args);
	void						setErrorPages(std::vector<std::string> args);
	// LocationBlock 만 해당
	void						setUri(std::string uri);
	void						setIndex(std::vector<std::string> args);
	void						setAutoIndex(std::vector<std::string> args);
	void						setCgi(std::vector<std::string> args);

	std::string					getListenPort();
	std::set<std::string>		getServerNames();
	std::string					getWebRoot();
	std::set<std::string>		getAllowedMethods();
	int							getClientBodySize();
	std::map<int, std::string>	getErrorPages();
	std::string					getErrorPage(int num);

	std::string					getUri();
	std::string					getIndex();
	std::string					getAutoIndex();
	std::set<std::string>		getCgi();

	void						printBlock();
};

#endif
