#include "./MessageParse.hpp"
#include "../color.hpp"

MessageParse::MessageParse(const std::string &message) :
	message_(message) {
}

MessageParse::~MessageParse() {}

void	MessageParse::tokenize() {
	std::string	token;
	std::istringstream	tokenStream(message_);

	while (std::getline(tokenStream, token, ' ')) {
		if (token.empty()) {
			continue;
		}
		tokens_.push_back(token);
	}
	return;
}

void	MessageParse::printTokens() const {
	for (std::vector<std::string>::const_iterator it = tokens_.begin(); \
			it != tokens_.end(); it++) {
		std::cout << GREEN << *it << END << " -> " << std::flush;
	}
	std::cout << std::endl;
	return;
}

int	main() {
	std::string	testMessageList[] = {
		"COMMAND param1 param2 param3",
		"   COMMAND    param1     param2   param3    ",
		"",
		"     "
	};

	for (size_t i = 0; i < sizeof(testMessageList) / sizeof(testMessageList[0]); i++) {
		std::cout << "Message: " << testMessageList[i] << std::endl;

		MessageParse	parser(testMessageList[i]);
		parser.tokenize();
		parser.printTokens();
	}
#ifdef LEAKS
	system("leaks -q parser");
#endif  // LEAKS
	return(0);
}
