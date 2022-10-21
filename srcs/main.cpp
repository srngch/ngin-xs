#include "Master.hpp"
#include "Request.hpp"
#include "config/Config.hpp"

int	main(int argc, char **argv, char **env) {
	if (argc != 2)
		return 1;
	try {	
		Config conf = Config();
		conf.parseConfigFile(argv[1]);

		Block serverblock = conf.getServerBlock(1111);
		Block locationblock = serverblock.getLocationBlock("/upload.py");
		locationblock.printBlock();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}

	try {
		Master master(env);

		// print env
		// char **envs = master.getEnv();
		// while (envs && *envs) {
		// 	std::cout << *envs << std::endl;
		// 	envs++;
		// }
		master.run();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
