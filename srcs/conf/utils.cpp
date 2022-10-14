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

std::vector<std::string> parseHostPort(const std::string &arg) {
	size_t						pos;
	std::vector<std::string>	args;

	pos = arg.find(":");
	// x.x.x.x:80 과 같은 형태가 아니면 에러
	if (pos == std::string::npos || pos == 0 || pos == arg.length() - 1)
		throw std::runtime_error("Wrong formatted configuration file: Listen x.x.x.x:port\n");
	// host(x.x.x.x)
	args.push_back(arg.substr(0, pos));
	// port(80)
	args.push_back(arg.substr(pos + 1, arg.length() - 1 - pos));

	return args;
}
