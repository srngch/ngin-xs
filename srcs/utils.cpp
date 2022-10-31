#include "utils.hpp"

vectorString split(const std::string &str, std::string delim) {
	vectorString	splitedMessage;
	std::size_t		index;
	std::string		tmp = str;
	std::string		s;

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

vectorString parseLine(const std::string &line, std::string delim) {
	vectorString	tokens;
	std::size_t		pos = 0;
	std::size_t		end;

	while (true) {
		end = line.find_first_of(delim, pos);
		if (end == std::string::npos) {
			break;
		}
		tokens.push_back(line.substr(pos, end - pos));
		pos = line.find_first_not_of(delim, end);
	}
	return tokens;
}

ft_bool isFileExist(const std::string &filePath)
{
    std::ifstream	file(filePath);
    return file.good();
}

std::string fileToString(const std::string &filePath) {
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

vectorChar fileToCharV(const std::string &filePath) {
	std::ifstream	openFile(filePath, std::ios::binary);

	vectorChar ret((std::istreambuf_iterator<char>(openFile)), std::istreambuf_iterator<char>());
	openFile.close();
	return ret;
}

ft_bool isIncluded(const std::string &value, const vectorString &array)
{
	return (std::find(array.begin(), array.end(), value) != array.end());
}

ft_bool isDirectory(const std::string &filePath)
{
	struct stat	buffer;

	if (stat(filePath.c_str(), &buffer) == -1)
		return FT_FALSE;
	if (buffer.st_mode & S_IFDIR)
		return FT_TRUE;
	return FT_FALSE;
}

std::string createPadding(int width, int length) {
	std::string	result;

	for (int i = 0; i < width - length; i++) {
		result += " ";
	}
	return result;
}

std::string createPaddedString(int width, const std::string &str) {
	return createPadding(width, str.length()) + str;
}

std::size_t hexStringToNumber(const std::string &s) {
	std::size_t			n;
	std::stringstream	ss;

	ss << std::hex << s;
	ss >> n;
	return n;
}

vectorChar stringToCharV(const std::string &s) {
	return vectorChar(s.begin(), s.end());
}

ft_bool hasWordInCharV(const vectorChar &src, const char *word) {
	vectorCharConstIter	it;

	it = std::search(src.begin(), src.end(), word, word + strlen(word));
	
	if (it == src.end())
		return FT_FALSE;
	return FT_TRUE;
}

void timestamp(const std::string &msg, timeval start, int socketId) {
	timeval	now;

	gettimeofday(&now, NULL);
	std::cerr <<  "(" << socketId << ") \t\t" << (now.tv_sec - start.tv_sec) * ONE_SEC_IN_MICROSEC + (now.tv_usec - start.tv_usec) << " :\t\t" << msg << std::endl;
}

void timestampNoSocket(const std::string &msg, timeval start) {
	timeval	now;

	gettimeofday(&now, NULL);
	std::cerr << (now.tv_sec - start.tv_sec) * ONE_SEC_IN_MICROSEC + (now.tv_usec - start.tv_usec) << " :\t\t" << msg << std::endl;
}
