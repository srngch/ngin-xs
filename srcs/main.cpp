#include "Master.hpp"
#include "Request.hpp"
#include "config/Config.hpp"

int	main(int argc, char **argv, char **env) {
	if (argc != 2)
		return 1;
	try {	
		Config conf = Config();
		conf.parseConfigFile(argv[1]);

		// segfault
		Block serverblock = conf.getServerBlock(1111);
		Block locationblock = serverblock.getLocationBlock("/upload.py");
		locationblock.printBlock();
	
		// /*.py 처럼 더 이상 자식 블록이 없는 인스턴스만 가져오면 세그폴트 안 남
		// Block location = conf.getServerBlock(8000).getLocationBlock("/");
		// location.printBlock();
		// conf.getServerBlock(8000).getLocationBlock("/upload.py").printBlock();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}

	try {
		Master master(env);
		// master.parseConfig(argv[1]);
		// Block serverblock = master.config_.getServerBlock(8000);
		// Block locationblock = serverblock.getLocationBlock("/upload.py");
		// locationblock.printBlock();

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
