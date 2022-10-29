#include "Nginxs.hpp"

timeval start;

int main(int argc, char **argv) {
	gettimeofday(&start, NULL);

	if (argc != 2) {
		std::cout << "Usage: ./webserv [CONF FILE NAME]" << std::endl;
		return EXIT_FAILURE;
	}
	try {
		Nginxs	nginxs(argv[1]);
		nginxs.run();
	} catch (std::exception &e) {
		std::cout << "main: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
