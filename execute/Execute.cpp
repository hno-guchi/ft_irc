#include "./Execute.hpp"
#include "../error/error.hpp"
#include "../server/Server.hpp"
#include "../reply/Reply.hpp"
#include "../user/User.hpp"

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
	"PING",
	"PONG",
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

int Execute::exec(User* user) {
	if (!this->isCommand()) {
		return (kERR_UNKNOWNCOMMAND);
	}
	// TODO(hnoguchi): Exec Command.
	if (this->command_.getCommand() == "PING") {
		std::string	message = ":" + user->getNickName() + " PONG ft_irc\r\n";
		std::cout << "Send message: [" << message << "]" << std::endl;
		ssize_t		sendMsgSize = sendNonBlocking(user->getFd(), message.c_str(), message.size());
		if (sendMsgSize <= 0) {
			// handleClientDisconnect(&this->fds_[clientIndex].fd);
			return (-1);
		}
		// TODO(hnoguchi): castは使わない実装にする？？
		if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
			fatalError("send");
		}
	}
	return (0);
}

#ifdef DEBUG

#include "../Parser/Parser.hpp"

int	main() {
#ifdef LEAKS
	system("leaks -q parser");
#endif  // LEAKS
	return(0);
}
#endif  // DEBUG
