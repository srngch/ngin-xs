#ifndef	__LOCATIONBLOCK_HPP__
# define __LOCATIONBLOCK_HPP__

# include <string>
# include <set>

class	LocationBlock {
private:
	std::string				index_;
	std::string				autoIndex_;
	std::set<std::string>	allowedMethods_;

public:
	LocationBlock();
	~LocationBlock();
};

#endif
