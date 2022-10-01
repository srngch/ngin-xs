#include "Master.hpp"
#include "Request.hpp"

#define DUMMY_REQ_MSG "POST /v1/login HTTP/1.1\r\nContent-Type: application/json; charset=utf-8\r\nHost: 127.0.0.1:8000\r\nConnection: close\r\nUser-Agent: Paw/3.4.0 (Macintosh; OS X/12.6.0) GCDHTTPRequest\r\nContent-Length: 55\r\n\r\n{\"username\":\"test\",\"hashed_password\":\"hashed_password\"}"

int	main() {
	// try {
	// 	Master master;
	// 	master.run();
	// } catch (std::exception &e) {
	// 	std::cout << e.what() << std::endl;
	// }
	Request req(DUMMY_REQ_MSG);
	return 0;
}
