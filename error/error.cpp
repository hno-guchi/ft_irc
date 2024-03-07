#include "./error.hpp"
#include "../color.hpp"

void	printErrorMessage(const std::string &message) {
	std::cerr << RED << message << END << std::endl;
	return;
}

void fatalError(const std::string& message) {
	std::perror(message.c_str());
	exit(EXIT_FAILURE);
}

