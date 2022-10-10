#include "Master.hpp"
#include "Request.hpp"

int	main(int argc, char **argv, char **env) {
	(void)argc;
	(void)argv;
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
