#include <iostream>
#include <vector>
#include <string>
#include "../user/User.hpp"

std::vector<std::string>	split(const std::string& message, const std::string delim, User* user) {
	try {
		std::vector<std::string>	messages;
		std::string::size_type		startPos(0);

		while (startPos < message.size()) {
			std::string::size_type	delimPos = message.find(delim, startPos);
			if (delimPos == message.npos && startPos < message.size()) {
				user->setLeftMsg(message.substr(startPos));
				break;
			}
			if (delimPos > startPos) {
				// TODO(hnoguchi): std::string::substr(); throw exception std::out_of_range();
				std::string	buf = message.substr(startPos, delimPos - startPos);
				messages.push_back(buf);
			}
			startPos = delimPos + delim.size();
		}
		return (messages);
	} catch (std::exception& e) {
		throw;
	}
}

#ifdef UTEST_SPLIT
#include "../color.hpp"

void	printMsg(const std::string& color, const std::string& msg) {
	for (size_t i = 0; i < msg.size(); i++) {
		if (msg[i] == '\r') {
			std::cout << RED << "\\r" << END << std::flush;
		} else if (msg[i] == '\n') {
			std::cout << RED << "\\n" << END << std::flush;
		} else if (!color.empty()) {
			std::cout << color << msg[i] << END << std::flush;
		} else {
			std::cout << msg[i] << std::flush;
		}
	}
}

void	printTestMsg(const std::string& msg) {
	std::cout << "testMsg_____________________" << std::endl;
	std::cout << "[" << std::flush;
	printMsg(GREEN, msg);
	std::cout << "]" << std::endl;
	// std::cout << std::endl;
	// std::cout << "----------------------------" << std::endl;
}

void	printSplited(const std::vector<std::string>& splited) {
	for (size_t i = 0; i < splited.size(); i++) {
		std::cout << "[" << std::flush;
		printMsg("", splited[i]);
		std::cout << "]" << std::endl;
	}
	// std::cout << std::endl;
}

int main() {
	std::string testList[] = {
		"COMMAND param\r\n",
		"COMMAND param\r\nCOMMAND param\r\n",
		"COMMAND param\r\nCOMMAND param\r\nCOMMAND param\r\n",
		"COMMAND param",
		"COMMAND param\r\nCOMMAND param",
		"COMMAND param\r\nCOMMAND param\r\nCOMMAND param",
		"COMMAND param\r",
		"COMMAND param\r\nCOMMAND param\r",
		"COMMAND param\r\nCOMMAND param\r\nCOMMAND param\r",
		"COMMAND param\n",
		"COMMAND param\r\nCOMMAND param\n",
		"COMMAND param\r\nCOMMAND param\r\nCOMMAND param\n",
		"COMMAND param\r\n\r",
		"COMMAND param\r\nCOMMAND param\r\n\r",
		"COMMAND param\r\nCOMMAND param\r\n\rCOMMAND param\r\n",
		"COMMAND param\r\n\r\nCOMMAND param\r\n",
	};

	std::string next("");
	for (size_t i = 0; i < sizeof(testList) / sizeof(testList[0]); i++) {
		printTestMsg(testList[i]);
		testList[i] = next + testList[i];
		next = "";
		printMsg(GREEN, testList[i]);
		std::cout << std::endl;
		std::vector<std::string>	msgs = split(testList[i], "\r\n", &next);
		printSplited(msgs);
		if (!next.empty()) {
			std::cout << "next: [" << std::flush;
			printMsg("", next);
			std::cout << "]" << std::endl;
		}
		std::cout << std::endl;
	}
}

#endif  // UTEST_SPLIT
