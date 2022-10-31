#ifndef __AUTOINDEX_HPP__
#define __AUTOINDEX_HPP__

#include <dirent.h>
#include <ctime>
#include "AutoindexData.hpp"
#include "../utils.hpp"

class Autoindex {
private:
	std::string					filePath_;
	std::string					uri_;
	std::vector<AutoindexData>	autoindexes_;
	std::string					html_;

	static ft_bool	comp(AutoindexData &a1, AutoindexData &a2);

	void	setAutoindexes(DIR *dir);
	void	setHtml();

public:
	Autoindex(const std::string &filePath, const std::string &uri);
	~Autoindex();

	std::string	getHtml();
};

#endif
