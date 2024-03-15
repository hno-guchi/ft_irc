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

// TODO(hnoguchi): 引数に、サーバのコンフィグやチャンネルの情報を渡す。
// TODO(hnoguchi): message paramのパースは、Parserクラスで行う。
int Execute::exec(User* user, std::vector<User>* users) {
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
	} else if (this->command_.getCommand() == "NOTICE") {
		// 送り主のニックネームを取得
		std::string	message = ":" + user->getNickName() + " NOTICE ";
		// 送り先のニックネームを取得
		std::vector<Param>	params = this->command_.getParams();
		// メッセージを付ける
		// TODO(hnoguchi): parser classで、prefixを外すかどうか考える必要がある。
		message += params[0].getValue() + " " + params[1].getValue() + "\r\n";
		for (std::vector<User>::const_iterator it = users->begin(); \
				it != users->end(); it++) {
			if (params[0].getValue() == it->getNickName()) {
				int fd = it->getFd();
				// 送信
				std::cout << "Send message: [" << message << "]" << std::endl;
				ssize_t		sendMsgSize = sendNonBlocking(fd, message.c_str(), message.size());
				if (sendMsgSize <= 0) {
					// handleClientDisconnect(&this->fds_[clientIndex].fd);
					return (-1);
				}
				// TODO(hnoguchi): castは使わない実装にする？？
				if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
					fatalError("send");
				}
				break;
			}
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
