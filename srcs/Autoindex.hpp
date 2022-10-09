#ifndef __AUTOINDEX_HPP__
#define __AUTOINDEX_HPP__

#include <string>
#include "macro.hpp"

class Autoindex {
private:
	std::string	name_;
	std::string	date_;
	std::size_t	size_;
	std::string	path_;
	ft_bool		isDirectory_;

public:
	Autoindex();
	~Autoindex();

	void	setName(const std::string &name);
	void	setDate(const std::string &date);
	void	setSize(std::size_t size);
	void	setPath(const std::string &path);
	void	setIsDirectory(ft_bool isDirectory);

	std::string	getName();
	std::string	getDate();
	std::size_t	getSize();
	std::string	getPath();
	ft_bool		getIsDirectory();
};

#endif
