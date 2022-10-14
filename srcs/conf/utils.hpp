#ifndef __CONF_UTILS_H__
# define __CONF_UTILS_H__

# include <vector>
# include <string>

std::vector<std::string>	parseLine(const std::string &str, std::string delim);
std::vector<std::string>	parseHostPort(const std::string &arg);

#endif
