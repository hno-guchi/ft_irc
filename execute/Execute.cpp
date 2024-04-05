#include "./Execute.hpp"
#include "../server/Server.hpp"
#include "../server/Info.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"
#include "../error/error.hpp"

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
int	Execute::exec(User* user, const ParsedMessage& parsedMsg, Info* info) {
	if (!this->isCommand(parsedMsg.getCommand(), info->getConfig().getCommandList())) {
		return (kERR_UNKNOWNCOMMAND);
	}
	if (parsedMsg.getCommand() == "PING") {
		return (pong(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "PASS") {
	// 	return (pass(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "NICK") {
	// 	return (nick(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "USER") {
	// 	return (user(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "OPER") {
	// 	return (oper(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "quit") {
	// 	return (quit(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "JOIN") {
	// 	return (join(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "KICK") {
	// 	return (kick(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "INVITE") {
	// 	return (invite(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "TOPIC") {
	// 	return (topic(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "MODE") {
	// 	return (mode(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "PRIVMSG") {
	// 	return (privmsg(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "NOTICE") {
	// 	return (notice(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "PONG") {
	// 	return (pong(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "ERROR") {
	// 	return (error(user, parsedMsg, info));
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
