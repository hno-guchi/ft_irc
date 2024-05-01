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

std::string	Execute::registerUser(User* user, const ParsedMessage& parsedMsg, Info* info) {
	if (!(user->getRegistered() & kPassCommand)) {
		if (parsedMsg.getCommand() == "PASS") {
			// std::string	reply = this->cmdPass(user, parsedMsg, info);
			std::string	reply;
			if (!reply.empty()) {
				return (reply);
			}
			user->setRegistered(kPassCommand);
			return ("");
		}
	} else if (!(user->getRegistered() & kNickCommand)) {
		if (parsedMsg.getCommand() == "NICK") {
			// std::string	reply = this->cmdNick(user, parsedMsg, info);
			std::string	reply;
			if (!reply.empty()) {
				return (reply);
			}
			user->setRegistered(kNickCommand);
			return ("");
		}
	} else if (!(user->getRegistered() & kUserCommand)) {
		if (parsedMsg.getCommand() == "USER") {
			// std::string	reply = this->cmdUser(user, parsedMsg, info);
			std::string	reply;
			if (!reply.empty()) {
				return (reply);
			}
			user->setRegistered(kUserCommand);
			return (Reply::rplWelcome(*info, *user));
		}
	}
	return (Reply::errNotRegistered(kERR_NOTREGISTERED, user->getNickName()));
}

// TODO(hnoguchi): exec();関数では、実行結果によるエラーを扱う。（例えば存在しないチャンネル名へのメッセージ送信など）
std::string	Execute::exec(User* user, const ParsedMessage& parsedMsg, Info* info) {
	// if (!this->isCommand(parsedMsg.getCommand(), info->getConfig().getCommandList())) {
	// 	return (Reply::errNotRegistered(kERR_UNKNOWNCOMMAND, user.getNickName(), parsedMsg.getCommand()));
	// }
	// if (parsedMsg.getCommand() == "PING") {
	// 	return (cmdPong(user, parsedMsg, info));
	if (parsedMsg.getCommand() == "PASS") {
		return (cmdPass(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "NICK") {
	// 	return (cmdNick(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "USER") {
	// 	return (cmdUser(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "OPER") {
	// 	return (cmdOper(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "QUIT") {
	// 	return (cmdQuit(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "JOIN") {
		return (cmdJoin(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "PART") {
	// 	return (cmdPart(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "KICK") {
	// 	return (cmdKick(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "INVITE") {
		return (cmdInvite(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "TOPIC") {
	// 	return (cmdTopic(user, parsedMsg, info));
	// TODO(hnoguchi): userMode();かchannelMode();なのか判定する処理が必要
	// TODO(hnoguchi): Paramsのtypeにchannelやuser
	} else if (parsedMsg.getCommand() == "MODE") {
		// for(std::vector<User>::const_iterator it = info->getUsers().begin(); it != info->getUsers().end(); it++) {
		// 	if (parsedMsg.getParams()[0].getValue() == it->getNickName()) {
		// 		return (cmdUserMode(user, parsedMsg, info));
		// 	}
		// }
		for(std::vector<Channel>::const_iterator it = info->getChannels().begin(); it != info->getChannels().end(); it++) {
			if (parsedMsg.getParams()[0].getValue() == it->getName()) {
				return (cmdChannelMode(user, parsedMsg, info));
			}
		}
		// TODO(hnoguchi): Channel Userのどちらにも該当しなかった場合の処理を追加すること
	// } else if (parsedMsg.getCommand() == "PRIVMSG") {
	// 	return (cmdPrivmsg(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "NOTICE") {
	// 	return (cmdNotice(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "PONG") {
	// 	return (pong(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "ERROR") {
	// 	return (error(user, parsedMsg, info));
	}
	return (Reply::errUnknownCommand(kERR_UNKNOWNCOMMAND, user->getNickName(), parsedMsg.getCommand()));
}
