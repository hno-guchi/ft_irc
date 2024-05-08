#include "./Parser.hpp"

const std::string	commandList_[] = {
	"PASS",
	"NICK",
	"USER",
	"OPER",
	"QUIT",
	"JOIN",
	"PART",
	"KICK",
	"INVITE",
	"TOPIC",
	"MODE",
	"PRIVMSG",
	"NOTICE",
	"PING",
	""
};

std::vector<std::string>	split(const std::string& message, const std::string delim) {
	std::vector<std::string>	messages;
	std::string::size_type		startPos(0);

	while (startPos < message.size()) {
		std::string::size_type	delimPos = message.find(delim, startPos);
		if (delimPos == message.npos) {
			break;
		}
		std::string	buf = message.substr(startPos, delimPos - startPos);
		messages.push_back(buf);
		startPos = delimPos + delim.size();
	}
	return (messages);
}

void	printMessages(const std::vector<std::string>& messages) {
	if (messages.empty()) {
		return;
	}
	std::cout << "[Splited]   ____________________" << std::endl;
	for (std::vector<std::string>::const_iterator it = messages.begin(); \
			it != messages.end(); it++) {
		std::cout << "[" << *it << "]" << std::endl;
	}
	std::cout << std::endl;
	return;
}

int	main() {
	std::string	testMessageList[] = {
		"COMMAND param1 :param2\r\nCOMMAND param1 :param2\r\nCOMMAND param1 :param2\r\n",
		"COMMAND param1 param2 param3\r\n",
		"   COMMAND   param1    param2     param3    \r\n",
		"",
		"     ",
		"COMMAND\r\n",
		"COMMAND \r\n",
		"command\r\n",
		"coMMAnd\r\n",
		"111\r\n",
		"11a \r\n",
		"11\r\n",
		"COMMAND param1 :param2\r\n",
		"COMMAND param1 :param2\r\nCOMMAND param1 :param2\r\nCOMMAND param1 :param2\r\n   COMMAND param1 :param2\r\nCOMMAND param1 :param2\r\n",
		"COMMAND param1 :param2 param3 param4 param5\r\nCOMMAND param1 :param2    param3\r\nCOMMAND param1 :param2 :param3 :param4\r\n   COMMAND param1 :param2     :param3     param4     :param5\r\nCOMMAND :param1 param2\r\n"
	};

	for (size_t i = 0; \
			i < sizeof(testMessageList) / sizeof(testMessageList[0]); i++) {
		std::cout << "[Message]    ____________________" << std::endl;
		std::cout << GREEN << testMessageList[i] << END << std::endl;
		std::cout << "_________________________________" << std::endl;

		// TODO(hnoguchi): crlfをデリミタに、メッセージの分割処理を実装する（複数のメッセージが一度に来る場合がある。）
		// TODO(hnoguchi): 終端にcrlfがあるかチェックする。
		// TODO(hnoguchi): メッセージの長さが512文字(crlfを含む)を超えていないかチェックする。

		std::vector<std::string>	messages = split(testMessageList[i], "\r\n");
		printMessages(messages);

		for (std::vector<std::string>::const_iterator it = messages.begin(); it != messages.end(); it++) {
			Parser	parser;

			// parser.tokenize();
			// parser.printTokens();
			parser.parse(*it);
			parser.getParsedMessage().printParsedMessage();
			std::cout << YELLOW << "[Execute]   ____________________" << END << std::endl;
			std::cout << YELLOW << "[Reply]     ____________________" << END << std::endl;
			std::cout << std::endl;
		}
	}
#ifdef LEAKS
	system("leaks -q parser");
#endif  // LEAKS
	return(0);
}
#endif  // DEBUG
