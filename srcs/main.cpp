#include "Master.hpp"
#include "Request.hpp"
#include "conf/Config.hpp"

int	main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	Config conf = Config(argv[1]);
	// try {
	// 	Master master;
	// 	master.run();
	// } catch (std::exception &e) {
	// 	std::cout << e.what() << std::endl;
	// }
	return 0;
}
