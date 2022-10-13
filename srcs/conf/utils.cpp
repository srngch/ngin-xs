#include "utils.hpp"

std::vector<std::string> parseLine(const std::string &line, std::string delim) {
	std::vector<std::string>	tokens;
	std::size_t					pos = 0;
	std::size_t					end;

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
