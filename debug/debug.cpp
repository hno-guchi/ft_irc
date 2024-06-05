#include "./debug.hpp"
#include "../color.hpp"

void	debugPrintErrorMessage(const std::string &message) {
	std::cerr << errno << " : " << std::flush;
	std::perror(message.c_str());
}

