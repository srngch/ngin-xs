#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include "macro.hpp"

template <typename T>
std::string ntos (T n)
{
	std::ostringstream	ss;

	ss << n;
	return ss.str();
}

std::vector<char>			splitVectorChar(const std::vector<char> &str, const char* delim);
std::vector<std::string>	split(const std::string &str, std::string delim);
// split 과 동일한 역할을 하지만, delimiter(예: abc)가 통째로 구분점이 되는 것이 아니라 각 문자(a, b, c)가 구분점이 된다는 것이 차이
std::vector<std::string>	parseLine(const std::string &str, std::string delim);
ft_bool						isFileExist(const std::string &filePath);
std::string					fileToString(const std::string &filePath);
std::vector<char>			fileToCharV(const std::string &filePath);
ft_bool						isIncluded(const std::string &value, const std::vector<std::string> &array);
ft_bool						isDirectory(const std::string &filePath);
ft_bool						isCgi(const std::string &filePath);
std::string					createPadding(int width, int length);
std::string					createPaddedString(int width, const std::string &str);
size_t						hexStringToNumber(std::string s);
std::vector<char>			stringToCharV(std::string s);

#endif
