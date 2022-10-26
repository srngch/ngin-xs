#include "Response.hpp"

Response::Response(std::string status) {
	statusLine_ = "HTTP/1.1 " + status;
	makeDefaultHeaders();
}

Response::Response(std::string status, const std::vector<char> &result, ft_bool isCgi) {
	// Set cgi headers
	std::vector<char>::const_iterator	it;
	const char 							*crlf = "\r\n\r\n";
	std::string 						cgiHeaders;
	std::vector<std::string>			splitedHeaders;
	std::vector<std::string>			splitedHeaderLine;
	std::vector<std::string>::iterator	it_s;

	statusLine_ = "HTTP/1.1 " + status;
	body_ = result;
	if (isCgi) {
		it = std::search(result.begin(), result.end(), crlf, crlf + strlen(crlf));
		if (it != result.end()) {
			body_ = std::vector<char>(it + strlen(crlf), result.end());
			cgiHeaders = std::string(result.begin(), it);
			// std::cout << "cgiHeaders: " << cgiHeaders << std::endl;
			splitedHeaders = split(std::string(cgiHeaders.begin(), cgiHeaders.end()), "\r\n");
			for (it_s = splitedHeaders.begin(); it_s != splitedHeaders.end(); it_s++) {
				splitedHeaderLine = split(*it_s, ": ");
				std::transform(splitedHeaderLine[0].begin(), splitedHeaderLine[0].end(), splitedHeaderLine[0].begin(), ::tolower);
				if (splitedHeaderLine[0] == "status") {
					statusLine_ = "HTTP/1.1 " + splitedHeaderLine[1];
				} else {
					appendHeader(splitedHeaderLine[0], splitedHeaderLine[1]);
				}
			}
		}
	}
	statusCode_ = std::atoi(statusLine_.substr(0, 3).c_str());
	makeDefaultHeaders();
}

Response::~Response() {}

std::string Response::makeDateValue() {
	std::time_t		t = std::time(0);
	struct tm		*tm = gmtime(&t);
	char			ret[30];

	strftime(ret, 30, "%a, %d %b %Y %H:%M:%S GMT", tm);
	return std::string(ret);
}

void Response::makeDefaultHeaders() {
	if (getHeaderValue("date").length() == 0)
		appendHeader("date", makeDateValue());
	if (getHeaderValue("server").length() == 0)
		appendHeader("server", "ngin-xs");
	if (getHeaderValue("content-length").length() == 0)
		appendHeader("content-length", ntos(body_.size()));
	if (getHeaderValue("content-type").length() == 0)
		appendHeader("content-type", MIME_HTML);
}

const std::string Response::getHeaderValue(std::string fieldName) {
	std::map<std::string, std::string>::iterator it = headers_.find(fieldName);
	if (it == headers_.end())
		return std::string("");
	return it->second;
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

void	Response::setContentType(std::string fileExtension) {
	headers_.erase("content-type");
	if (fileExtension == "html")
		appendHeader("content-type", MIME_HTML);
	else if (fileExtension == "css")
		appendHeader("content-type", MIME_CSS);
	else if (fileExtension == "gif")
		appendHeader("content-type", MIME_IMAGE_GIF);
	else if (fileExtension == "jpeg" || fileExtension == "jpg")
		appendHeader("content-type", MIME_IMAGE_JPG);
	else if (fileExtension == "js")
		appendHeader("content-type", MIME_APP_JS);
	else if (fileExtension == "txt")
		appendHeader("content-type", MIME_TEXT);
	else if (fileExtension == "png")
		appendHeader("content-type", MIME_IMAGE_PNG);
	else if (fileExtension == "svg" || fileExtension == "svgz")
		appendHeader("content-type", MIME_IMAGE_SVG);
	else if (fileExtension == "ico")
		appendHeader("content-type", MIME_IMAGE_ICO);
	else if (fileExtension == "json")
		appendHeader("content-type", MIME_APP_JSON);
	else if (fileExtension == "pdf")
		appendHeader("content-type", MIME_APP_PDF);
	else if (fileExtension == "zip")
		appendHeader("content-type", MIME_APP_ZIP);
	else // .bin .exe .dll or unknown MIME
		appendHeader("content-type", MIME_OCTET);
}

void	Response::appendHeader(std::string fieldName, std::string value) {
	headers_.insert(std::pair<std::string, std::string>(fieldName, value));
}
