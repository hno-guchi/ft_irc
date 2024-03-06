#include "./Execute.hpp"
#include "../reply/Reply.hpp"

const std::string	Execute::cmdList_[] = {
	"PASS",
	"NICK",
	"USER",
	"QUIT",
	"JOIN",
	"KICK",
	"INVITE",
	"TOPIC",
	"MODE",
	"PRIVMSG",
	"NOTICE",
	""
};

Execute::Execute(const Command& command) : command_(command) {}
Execute::~Execute() {}

bool Execute::isCommand() {
	for (int i = 0; this->cmdList_[i] != ""; i++) {
		if (this->cmdList_[i] == this->command_.getCommand()) {
			return (true);
		}
	}
	return (false);
}

int Execute::exec() {
	if (!this->isCommand()) {
		return (kERR_UNKNOWNCOMMAND);
	}
	// TODO(hnoguchi): Exec Command.
	return (kERR_UNKNOWNCOMMAND);
	// return (0);
}

#ifdef DEBUG

#include "../Parser/Parser.hpp"

int	main() {
	std::string	testMessageList[] = {
		"COMMAND\r\n",
		"COMMAND\r\n",
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
		"COMMAND param1 :param2\r\nCOMMAND param1 :param2\r\nCOMMAND param1 :param2\r\n   COMMAND param1 :param2\r\nCOMMAND param1 :param2\r\n"
	};

	for (size_t i = 0; \
			i < sizeof(testMessageList) / sizeof(testMessageList[0]); i++) {
		std::cout << GREEN << "Message: [" << testMessageList[i] << "]" << END << std::endl;

		// TODO(hnoguchi): crlfをデリミタに、メッセージの分割処理を実装する（複数のメッセージが一度に来る場合がある。）
		// TODO(hnoguchi): 終端にcrlfがあるかチェックする。
		// TODO(hnoguchi): メッセージの長さが512文字(crlfを含む)を超えていないかチェックする。

		std::vector<std::string>	messages = split(testMessageList[i], "\r\n");
		printMessages(messages);

		for (std::vector<std::string>::const_iterator it = messages.begin(); it != messages.end(); it++) {
			Parser	parser(*it);

			parser.tokenize();
			parser.printTokens();

			parser.parse();
			parser.getCommand().printCommand();
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
