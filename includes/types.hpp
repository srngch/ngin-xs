#ifndef	__TYPES_HPP__
# define __TYPES_HPP__

#include <vector>
#include <set>
#include <map>

typedef std::vector<char>				vectorChar;
typedef vectorChar::iterator			vectorCharIter;
typedef vectorChar::const_iterator		vectorCharConstIter;

typedef std::vector<std::string>		vectorString;
typedef vectorString::iterator			vectorStringIter;
typedef vectorString::reverse_iterator	vectorStringReverseIter;
typedef vectorString::const_iterator	vectorStringConstIter;

typedef std::set<std::string>		setString;
typedef setString::iterator			setStringIter;
typedef setString::const_iterator	setStringConstIter;

typedef std::map<std::string, std::string>	mapStringString;
typedef mapStringString::iterator			mapStringStringIter;

typedef std::map<int, std::string> 	mapIntString;
typedef mapIntString::iterator		mapIntStringIter;

#endif
