#include "Master.hpp"
#include "Request.hpp"
#include "config/Config.hpp"

int	main(int argc, char **argv, char **env) {
	if (argc != 2)
		return 1;
	try {
		// std::vector<Block>				serverblocks;
		std::vector<Block>::iterator	it;
	
		Config conf = Config(argv[1]);
		// serverblocks = conf.getServerBlocks();
		// for (it = serverblocks.begin(); it != serverblocks.end(); it++)
		// 	it->printBlock();
		Block serverblock = conf.getServerBlock(8000);
		// serverblock.printBlock();
		Block	locationblock = serverblock.getLocationBlock("/");
		std::cout <<  "@@@@ " << serverblock.getLocationBlock("/").getParent()->getParent() << std::endl;
		std::cout << "---- " << locationblock.getParent()->getParent() << std::endl;
		std::cout << locationblock.getParent() << std::endl;
		std::cout << locationblock.getHost() << std::endl;
		std::cout << locationblock.getPort() << std::endl;
		// locationblock.printBlock();
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
