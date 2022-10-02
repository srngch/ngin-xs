#include "utils.hpp"

std::vector<std::string> split(const std::string &str, std::string delim) {
	std::vector<std::string>	splitedMessage;
	std::size_t 				index;
	std::string					tmp = str;
	std::string					s;

	while (FT_TRUE) {
		if (tmp.find(delim) == 0)
			tmp = tmp.substr(delim.length(), std::string::npos);
		index = tmp.find(delim);
		s = tmp.substr(0, index);
		splitedMessage.push_back(s);
		if (index == std::string::npos)
			break ;
		tmp = tmp.substr(index, std::string::npos);
	}
	return splitedMessage;
}

std::string fileToString(const std::string filePath) {
	std::string		str = "";
	std::ifstream	openFile(filePath);

	if(openFile.is_open()) {
		std::string line;
		while(std::getline(openFile, line)) {
			str += line + "\n";
		}
		openFile.close();
	}
	return str;
}

ft_bool isIncluded(const std::string &value, const std::vector<std::string> &array)
{
	return (std::find(array.begin(), array.end(), value) != array.end());
}
