#include "./error.hpp"
#include "../color.hpp"

void	debugPrintSendMessage(const std::string& prefix, const std::string &message) {
	(void)prefix;
	// std::cout << "[" << prefix << "] _______________________________" << std::endl;
	// std::cout << "[" << prefix << "] [" << std::flush;
	// std::cout << YELLOW << message << END << "]" << std::endl;
	std::cout << YELLOW << message << END << std::flush;
	// std::cout << "------------------------------------------" << std::endl;
}

void	printErrorMessage(const std::string &message) {
	std::cerr << RED << message << END << std::endl;
}

void	fatalError(const std::string& message) {
	std::cerr << errno << " : " << std::flush;
	std::perror(message.c_str());
	// exit(EXIT_FAILURE);
}

