#include "ServerBlock.hpp"

ServerBlock::ServerBlock() {
	listenPort_ = 0;
	clientBodyMaxSize_ = 0;
	serverNames_ = {};
	webRoot_ = "";
	cgi_ = {};
	allowedMethods_ = {};
	errorPages_ = {};
	locations_ = {};
}

ServerBlock::~ServerBlock() {}

directivesMap ServerBlock::getDirectivesMap() {
	directivesMap	tmpDirectivesMap;

	tmpDirectivesMap["listen"] = &ServerBlock::setListenPort;
	tmpDirectivesMap["server_name"] = &ServerBlock::setServerName;
	tmpDirectivesMap["root"] = &ServerBlock::setWebRoot;
	tmpDirectivesMap["cgi"] = &ServerBlock::setCgi;
	tmpDirectivesMap["limit_except"] = &ServerBlock::setAllowedMethods;
	tmpDirectivesMap["client_body_buffer_size"] = &ServerBlock::setClientBodySize;

	return tmpDirectivesMap;
}

ft_bool ServerBlock::has_semi_colon(std::vector<std::string> &tokens, int &index) {
	int	last;
	directivesMap::iterator		it;

	last = tokens[index].length() - 1;
	if (tokens[last] == ";") {
		tokens[index].erase(last, last);
		// xxx; 뒤에 오는 토큰이 없이 파일의 끝이면 에러
		if (index + 1 == tokens.size())
			throw std::runtime_error("Invalid configuration file.\n");
		if (tokens[index + 1] == "}")
			return FT_TRUE;
		it = getDirectivesMap().find(tokens[index + 1]);
		if (it == getDirectivesMap().end())
			throw std::runtime_error("Invalid configuration file.\n");
		return FT_TRUE;
	}
	return FT_FALSE;
}

void ServerBlock::parseServerBlock(std::vector<std::string> &tokens, int &index) {
	int							tokenSize;
	directivesMap::iterator		it;
	std::string					directive = "";
	std::vector<std::string>	args;

	tokenSize = tokens.size();
	while (index < tokenSize && tokens[index] != "}")
	{
		it = getDirectivesMap().find(tokens[index]);
		// 토큰이 directivesMap 의 키(listen, server_name, ...)에 해당하지 않음
		if (it == getDirectivesMap().end()) {
			if (tokens[index] == "location") {
				LocationBlock	tmpLocationBlock;
				tmpLocationBlock.parseLocationBlock(tokens, index);
				locations_.push_back(tmpLocationBlock);
			}
			else {
				if (has_semi_colon(tokens, index)) {
					args.push_back(tokens[index]);
					(this->*ServerBlock::getDirectivesMap()[directive])(args);
					directive = "";
					args.clear();
				}
				else
					args.push_back(tokens[index]);
			}
		} else
			directive = tokens[index];
		index++;
	}
}

void ServerBlock::setListenPort(std::vector<std::string> args) {
	if (args.size() != 1)
		throw std::runtime_error("Invalid configuration file: port\n");
	listenPort_ = args[0];
}

void	ServerBlock::setServerName(std::vector<std::string> args) {
	std::vector<std::string>::iterator	it;

	if (args.empty())
		throw std::runtime_error("Invalid configuration file: server name\n");
	for (it = args.begin(); it != args.end(); it++)
		serverNames_.insert(*it);
}

void	ServerBlock::setWebRoot(std::vector<std::string> args) {
	if (args.size() != 1)
		throw std::runtime_error("Invalid configuration file: port\n");
	webRoot_ = args[0];
}

void	ServerBlock::setCgi(std::vector<std::string> args) {
	std::vector<std::string>::iterator	it;

	if (args.empty())
		throw std::runtime_error("Invalid configuration file: server name\n");
	for (it = args.begin(); it != args.end(); it++)
		serverNames_.insert(*it);
}

void	ServerBlock::setAllowedMethods(std::vector<std::string> args) {

}

void	ServerBlock::setClientBodySize(std::vector<std::string> args) {

}
