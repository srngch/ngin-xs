#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "macro.hpp"

template <typename T>
std::string ntos (T n)
{
	std::ostringstream	ss;

	ss << n;
	return ss.str();
}

std::vector<std::string> split(const std::string &str, std::string delim);
std::string fileToString(const std::string filePath);

#endif
