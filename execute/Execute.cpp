#include "./Execute.hpp"
#include "../server/Server.hpp"
#include "../server/Info.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"
#include "../error/error.hpp"
#include "../parser/Parser.hpp"

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

int	Execute::registerUser(User* user, const ParsedMessage& parsedMsg, Info* info) {
	if (!(user->getRegistered() & kPassCommand)) {
		if (parsedMsg.getCommand() == "PASS") {
			int replyNum = this->cmdPass(user, parsedMsg, info);
			if (replyNum > 400) {
				return (replyNum);
			}
			user->setRegistered(kPassCommand);
			return (0);
		}
	} else if (!(user->getRegistered() & kNickCommand)) {
		if (parsedMsg.getCommand() == "NICK") {
			int replyNum = this->cmdNick(user, parsedMsg, info);
			if (replyNum > 400) {
				return (replyNum);
			}
			user->setRegistered(kNickCommand);
			return (0);
		}
	} else if (!(user->getRegistered() & kUserCommand)) {
		if (parsedMsg.getCommand() == "USER") {
			int replyNum = this->cmdUser(user, parsedMsg, info);
			if (replyNum > 400) {
				return (replyNum);
			}
			user->setRegistered(kUserCommand);
			return (kRPL_WELCOME);
		}
	}
	return (kERR_NOTREGISTERED);
}

// TODO(hnoguchi): exec();関数では、実行結果によるエラーを扱う。（例えば存在しないチャンネル名へのメッセージ送信など）
int	Execute::exec(User* user, const ParsedMessage& parsedMsg, Info* info) {
	if (!this->isCommand(parsedMsg.getCommand(), info->getConfig().getCommandList())) {
		return (kERR_UNKNOWNCOMMAND);
	}
	if (parsedMsg.getCommand() == "PING") {
		return (cmdPong(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "PASS") {
		return (cmdPass(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "NICK") {
		return (cmdNick(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "USER") {
		return (cmdUser(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "OPER") {
		return (cmdOper(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "QUIT") {
		return (cmdQuit(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "JOIN") {
		return (cmdJoin(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "PART") {
		return (cmdPart(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "KICK") {
		return (cmdKick(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "INVITE") {
		return (cmdInvite(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "TOPIC") {
		return (cmdTopic(user, parsedMsg, info));
	// 	TODO(hnoguchi): userMode();かchannelMode();なのか判定する処理が必要
	// 	TODO(hnoguchi): Paramsのtypeにchannelやuser
	} else if (parsedMsg.getCommand() == "MODE") {
		for(std::vector<User>::const_iterator it = info->getUsers().begin(); it != info->getUsers().end(); it++) {
			if (parsedMsg.getParams()[0].getValue() == it->getNickName()) {
				return (cmdUserMode(user, parsedMsg, info));
			}
		}
		for(std::vector<Channel>::const_iterator it = info->getChannels().begin(); it != info->getChannels().end(); it++) {
			if (parsedMsg.getParams()[0].getValue() == it->getName()) {
				return (cmdChannelMode(user, parsedMsg, info));
			}
		}
		// 	TODO(hnoguchi): Channel Userのどちらにも該当しなかった場合の処理を追加すること
	} else if (parsedMsg.getCommand() == "PRIVMSG") {
		return (cmdPrivmsg(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "NOTICE") {
		return (cmdNotice(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "PONG") {
	// 	return (pong(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "ERROR") {
	// 	return (error(user, parsedMsg, info));
	}
	return (kERR_UNKNOWNCOMMAND);
}

#ifdef DEBUG

#include "../parser/Parser.hpp"

int	main() {
#ifdef LEAKS
	system("leaks -q parser");
#endif  // LEAKS
	return(0);
}
#endif  // DEBUG
