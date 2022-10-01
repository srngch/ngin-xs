#include "Master.hpp"
#include "Request.hpp"

int	main() {
	try {
		Master master;
		master.run();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
