#include "Response.hpp"

Response::Response(std::string status) {
	statusCode_ = std::atoi(status.substr(0, 3).c_str());
	statusLine_ = "HTTP/1.1 " + status;
	makeDefaultHeaders();
}

Response::Response(std::string status, const std::vector<char> &body) {
	statusCode_ = std::atoi(status.substr(0, 3).c_str());
	statusLine_ = "HTTP/1.1 " + status;
	body_ = body;
	makeDefaultHeaders();
}

Response::~Response() {}

void Response::makeDefaultHeaders() {
	// TODO: date value: strftime()
	// headers_.insert(std::pair<std::string, std::string>("date", ));
	headers_.insert(std::pair<std::string, std::string>("server", "ngin-xs"));
	headers_.insert(std::pair<std::string, std::string>("content-length", ntos(body_.size())));
	// headers_.insert(std::pair<std::string, std::string>("content-type", ));
}

std::vector<char>	Response::createMessage() {
	std::vector<char>								message;
	std::map<std::string, std::string>::iterator	it;
	const char 										*crlf = CRLF;
	const char 										*headerDelim = ": ";

	message.insert(message.end(), statusLine_.begin(), statusLine_.end());
	message.insert(message.end(), crlf, crlf + strlen(crlf));
	for (it = headers_.begin(); it != headers_.end(); it++) {
		message.insert(message.end(), it->first.begin(), it->first.end());
		message.insert(message.end(), headerDelim, headerDelim + strlen(headerDelim));
		message.insert(message.end(), it->second.begin(), it->second.end());
		message.insert(message.end(), crlf, crlf + strlen(crlf));
	}
	message.insert(message.end(), crlf, crlf + strlen(crlf));
	message.insert(message.end(), body_.begin(), body_.end());
	return message;
}

void	Response::appendHeader(std::string fieldName, std::string value) {
	headers_.insert(std::pair<std::string, std::string>(fieldName, value));
}
