#include "Master.hpp"
#include "Request.hpp"
#include "conf/Config.hpp"

int	main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	try {
		Config conf = Config(argv[1]);
	}
	catch (std::exception &e){
		e.what()
	}
	// try {
	// 	Master master;
	// 	master.run();
	// } catch (std::exception &e) {
	// 	std::cout << e.what() << std::endl;
	// }
	return 0;
}
