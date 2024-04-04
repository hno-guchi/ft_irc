#include "./Execute.hpp"
#include "../error/error.hpp"
#include "../server/Server.hpp"
#include "../server/Info.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"

Execute::Execute() {}
Execute::~Execute() {}

bool	Execute::isCommand(const std::string& command, const std::string* cmdList) {
	for (int i = 0; cmdList[i] != ""; i++) {
		if (command == cmdList[i]) {
			return (true);
		}
	}
	return (false);
}

// TODO(hnoguchi): 引数に、サーバのコンフィグやチャンネルの情報を渡す。
// TODO(hnoguchi): message paramのパースは、Parserクラスで行う。
int	Execute::exec(User* user, const Command& command, Info* info) {
	if (!this->isCommand(command.getCommand(), info->getConfig().getCommandList())) {
		return (kERR_UNKNOWNCOMMAND);
	}
	// TODO(hnoguchi): Exec Command.
	if (command.getCommand() == "PING") {
		std::string	message = ":" + user->getNickName() + " PONG ft_irc\r\n";
		std::cout << "Send message: [" << message << "]" << std::endl;
		ssize_t		sendMsgSize = sendNonBlocking(user->getFd(), message.c_str(), message.size());
		if (sendMsgSize <= 0) {
			// handleClientDisconnect(&user->getFd());
			return (-1);
		}
		// TODO(hnoguchi): castは使わない実装にする？？
		if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
			fatalError("send");
		}
	} else if (command.getCommand() == "NOTICE") {
		// 送り主のニックネームを取得
		std::string	message = ":" + user->getNickName() + " NOTICE ";
		// 送り先のニックネームを取得
		std::vector<Param>	params = command.getParams();
		// メッセージを付ける
		// TODO(hnoguchi): parser classで、prefixを外すかどうか考える必要がある。
		message += params[0].getValue() + " " + params[1].getValue() + "\r\n";
		for (std::vector<User>::const_iterator it = info->getUsers().begin(); \
				it != info->getUsers().end(); it++) {
			if (params[0].getValue() == it->getNickName()) {
				int fd = it->getFd();
				// 送信
				std::cout << "Send message: [" << message << "]" << std::endl;
				ssize_t		sendMsgSize = sendNonBlocking(fd, message.c_str(), message.size());
				if (sendMsgSize <= 0) {
					// handleClientDisconnect(&it->getFd);
					return (-1);
				}
				// TODO(hnoguchi): castは使わない実装にする？？
				if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
					fatalError("send");
				}
				break;
			}
		}
	} else if (command.getCommand() == "PRIVMSG") {
		// channelがなければ、エラーリプライナンバーを返す
		// channelを探す
		for (std::vector<Channel>::const_iterator it = info->getChannels().begin(); \
				it != info->getChannels().end(); it++) {
			// channelがあれば、メンバーにメッセージを送信する
			if (command.getParams()[0].getValue() == it->getName()) {
				// TODO(hnoguchi): このやり方は、良くないと思うので、改善が必要。
				const std::vector<User*>& members = it->getMembers();
				for (std::vector<User*>::const_iterator member = members.begin(); \
						member != members.end(); member++) {
					if (user->getNickName() == (*member)->getNickName()) {
						continue;
					}
					// 送り主のニックネームを取得
					std::string	message = ":" + user->getNickName() + " PRIVMSG ";
					// 送り先のchannelを取得
					message += command.getParams()[0].getValue() + " " + command.getParams()[1].getValue() + "\r\n";
					std::cout << "Send message: [" << message << "]" << std::endl;
					ssize_t		sendMsgSize = sendNonBlocking((*member)->getFd(), message.c_str(), message.size());
					if (sendMsgSize <= 0) {
						// handleClientDisconnect(&(*member)->getFd());
						return (-1);
					}
					// TODO(hnoguchi): castは使わない実装にする？？
					if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
						fatalError("send PRIVMSG");
					}
				}
			}
		}
	} else if (command.getCommand() == "JOIN") {
		// channelを探す
		for (std::vector<Channel>::iterator it = const_cast<std::vector<Channel> &>(info->getChannels()).begin(); \
				it != const_cast<std::vector<Channel> &>(info->getChannels()).end(); it++) {
			// あれば、追加して、リプライナンバーを返す
			if (command.getParams()[0].getValue() == it->getName()) {
				it->addMember(user);
				return (kRPL_TOPIC);
			}
		}
		// なければchannelを作成して、ユーザを追加して、リプライナンバーを返す
		info->addChannel(Channel(command.getParams()[0].getValue()));
		for (std::vector<Channel>::iterator it = const_cast<std::vector<Channel> &>(info->getChannels()).begin(); \
				it != const_cast<std::vector<Channel> &>(info->getChannels()).end(); it++) {
			// あれば、追加して、リプライナンバーを返す
			if (command.getParams()[0].getValue() == it->getName()) {
				it->addMember(user);
				return (kRPL_TOPIC);
			}
		}
	// } else if (command.getCommand() == "PART") {
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
