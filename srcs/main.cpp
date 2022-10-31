#include "Nginxs.hpp"

timeval start;

int main(int argc, char **argv) {
	try {
		if (argc > 2) {
			std::cout << "Usage: ./webserv [CONF_FILE_NAME]" << std::endl;
			return EXIT_FAILURE;
		}
		gettimeofday(&start, NULL);
		Nginxs	nginxs(argv[1]);
		nginxs.run();
	} catch (std::exception &e) {
		std::cout << "main: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
