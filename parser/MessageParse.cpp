#include "MessageParse.hpp"

MessageParse::MessageParse(const std::string &message) :
	message_(message) {
}

MessageParse::~MessageParse() {}

void	MessageParse::tokenize() {
	return;
}

void	MessageParse::printTokens() const {
	return;
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <message>" << std::endl;
		return(1);
	}
	MessageParse	parser(argv[1]);

	parser.tokenize();
	parser.printTokens();

	system("leaks -q messageparse");
	return(0);
}
