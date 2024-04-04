#include "./Execute.hpp"
#include "../server/Server.hpp"
#include "../server/Info.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"
#include "../error/error.hpp"
#include "../command/command.hpp"

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

// TODO(hnoguchi): exec();関数では、実行結果によるエラーを扱う。（例えば存在しないチャンネル名へのメッセージ送信など）
int	Execute::exec(User* user, const Command& command, Info* info) {
	if (!this->isCommand(command.getCommand(), info->getConfig().getCommandList())) {
		return (kERR_UNKNOWNCOMMAND);
	}
	if (command.getCommand() == "PING") {
		return (pong(user, command, info));
	// } else if (command.getCommand() == "PASS") {
	// 	return (pass(user, command, info));
	// } else if (command.getCommand() == "NICK") {
	// 	return (nick(user, command, info));
	// } else if (command.getCommand() == "USER") {
	// 	return (user(user, command, info));
	// } else if (command.getCommand() == "OPER") {
	// 	return (oper(user, command, info));
	// } else if (command.getCommand() == "quit") {
	// 	return (quit(user, command, info));
	// } else if (command.getCommand() == "JOIN") {
	// 	return (join(user, command, info));
	// } else if (command.getCommand() == "KICK") {
	// 	return (kick(user, command, info));
	// } else if (command.getCommand() == "INVITE") {
	// 	return (invite(user, command, info));
	// } else if (command.getCommand() == "TOPIC") {
	// 	return (topic(user, command, info));
	// } else if (command.getCommand() == "MODE") {
	// 	return (mode(user, command, info));
	// } else if (command.getCommand() == "PRIVMSG") {
	// 	return (privmsg(user, command, info));
	// } else if (command.getCommand() == "NOTICE") {
	// 	return (notice(user, command, info));
	// } else if (command.getCommand() == "PONG") {
	// 	return (pong(user, command, info));
	// } else if (command.getCommand() == "ERROR") {
	// 	return (error(user, command, info));
	}
	return (kERR_UNKNOWNCOMMAND);
	// return (0);
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
