#ifndef __BLOCK_HPP__
# define __BLOCK_HPP__

# include <cstdlib> 
# include <iostream>
# include <set>
# include <string>
# include <vector>
# include <map>
# include "../macro.hpp"
# include "../utils.hpp"

#define directivesMap std::map<std::string, void (Block::*)(std::vector<std::string>)>

class Block {
private:
	static Block						defaultBlock_;
	Block								*parent_;
	std::vector<Block *>				locationBlocks_;
	directivesMap						directivesMap_;

	std::string							host_;
	int									port_;
	std::set<std::string>				serverNames_;
	std::string							webRoot_;
	std::set<std::string>				allowedMethods_;
	int									clientMaxBodySize_;
	std::map<int, std::string>			errorPages_;
	std::string							uri_;
	std::string							index_;
	std::string							autoIndex_;
	std::string							cgi_;

	ft_bool								check_validation(std::vector<std::string> &tokens, int &index, std::string &directive);
	std::vector<std::string>			parseHostPort(const std::string &arg);

public:
	Block();
	Block(Block *parent);
	Block &operator=(const Block &origin);

	static void							setDefaultBlock(const char *file);
	void								setDefaultServerDirectivesMap();
	void								setServerDirectivesMap();
	void								setLocationDirectivesMap();
	directivesMap						getDirectivesMap();
	
	ft_bool								has_semi_colon(std::vector<std::string> &tokens, int &index, std::vector<std::string> *args, std::string &directive);
	void								parseServerBlock(std::vector<std::string> &tokens, int &index);
	void								parseLocationBlock(std::vector<std::string> &tokens, int &i);
	
	// ServerBlock 만 해당
	void								setHostPort(std::vector<std::string> args);
	void								setHost(std::string host);
	void								setPort(int port);
	void								setServerNames(std::vector<std::string> args);
	// ServerBlock, LocationBlock 공통
	void								setWebRoot(std::vector<std::string> args);
	void								setAllowedMethods(std::vector<std::string> args);
	void								setClientMaxBodySize(int size);
	void								setClientMaxBodySize(std::vector<std::string> args);
	void								setErrorPages(std::vector<std::string> args);
	// LocationBlock 만 해당
	void								setParent(Block *parent);
	void								setUri(std::string uri);
	void								setIndex(std::vector<std::string> args);
	void								setAutoIndex(std::vector<std::string> args);
	void								setCgi(std::vector<std::string> args);

	const Block							&getParent() const;
	const std::vector<Block *>			&getLocationBlocks() const;
	const std::string					&getHost() const;
	const int							&getPort() const;
	const std::set<std::string>			&getServerNames() const;
	const std::string					&getWebRoot() const;
	const std::set<std::string>			&getAllowedMethods() const;
	const int							&getClientMaxBodySize() const;
	const std::map<int, std::string>	&getErrorPages() const;
	const std::string					&getErrorPage(int num) const;

	const std::string					&getUri() const;
	const std::string					&getIndex() const; 
	const std::string					&getAutoIndex() const;
	const std::string					&getCgi() const;

	const Block							&getLocationBlock(std::string uri);
	void								printBlock() const;

	class InvalidConfigFileException : public std::exception {
	private:
		std::string						message_;
	public:
		InvalidConfigFileException(const std::string msg);
		virtual const char *what() const throw();
		~InvalidConfigFileException() throw();
	};

	class InvalidLocationBlockException : public std::exception {
		virtual const char *what() const throw();
	};

};

#endif
