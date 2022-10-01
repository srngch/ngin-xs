#include "Response.hpp"

Response::Response(std::string status, std::string body) {
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
	headers_.insert(std::pair<std::string, std::string>("content-length", ntos(body_.length())));
	// headers_.insert(std::pair<std::string, std::string>("content-type", ));

}

std::string	Response::createMessage() {
	std::string	message;
	std::multimap<std::string, std::string>::iterator	it;

	message = statusLine_ + CRLF;
	for (it = headers_.begin(); it != headers_.end(); it++)
		message += it->first + ": " + it->second + CRLF;
	message += CRLF;
	message += body_;
	return message;
}
