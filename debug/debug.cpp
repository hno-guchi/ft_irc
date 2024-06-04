#include "./debug.hpp"
#include "../color.hpp"

void	debugPrintSendMessage(const std::string& to, const std::string &message) {
	// std::cout << "[" << prefix << "] _______________________________" << std::endl;
	// std::cout << "[" << prefix << "] [" << std::flush;
	// std::cout << YELLOW << message << END << "]" << std::endl;
	std::cout << "To  : " << to << " : " << YELLOW << message << END << std::flush;
	// std::cout << "------------------------------------------" << std::endl;
}

void	debugPrintRecvMessage(const std::string& from, const std::string &message) {
	std::cout << "From: " << from << " : " << GREEN << message << END << std::flush;
}

void	debugPrintErrorMessage(const std::string &message) {
	std::cerr << errno << " : " << std::flush;
	std::perror(message.c_str());
}


