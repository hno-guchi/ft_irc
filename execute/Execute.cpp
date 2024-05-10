#include "./Execute.hpp"
#include "../server/Server.hpp"
#include "../server/Info.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"
#include "../error/error.hpp"
#include "../parser/Parser.hpp"
#include "../parser/ParsedMsg.hpp"

Execute::Execute() {}
Execute::~Execute() {}

std::string	Execute::registerUser(User* user, const ParsedMsg& parsedMsg, Info* info) {
	if (!(user->getRegistered() & kPassCommand) && parsedMsg.getCommand() == "PASS") {
		std::string	reply = this->cmdPass(user, parsedMsg, info);
		if (reply.empty()) {
			user->setRegistered(kPassCommand);
		}
		// throw error;
		return ("");
	}
	if (!(user->getRegistered() & kNickCommand) && parsedMsg.getCommand() == "NICK") {
		if (!(user->getRegistered() & kPassCommand)) {
			// throw error;
			return ("");
		}
		std::string	reply = this->cmdNick(user, parsedMsg, info);
		if (reply.empty()) {
			user->setRegistered(kNickCommand);
		}
		// throw error;
		return ("");
	}
	if (!(user->getRegistered() & kUserCommand) && parsedMsg.getCommand() == "USER") {
		if (!(user->getRegistered() & kPassCommand) || !(user->getRegistered() & kNickCommand)) {
			// throw error;
			return ("");
		}
		std::string	reply = this->cmdUser(user, parsedMsg, info);
		if (reply.empty()) {
			user->setRegistered(kUserCommand);
			return (Reply::rplWelcome(*info, *user));
		}
	}
	// throw error;
	return (Reply::errNotRegistered(kERR_NOTREGISTERED, user->getNickName()));
}

// TODO(hnoguchi): exec();関数では、実行結果によるエラーを扱う。（例えば存在しないチャンネル名へのメッセージ送信など）
std::string	Execute::exec(User* user, const ParsedMsg& parsedMsg, Info* info) {
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
	// TODO(hnoguchi): Paramsのtypeにchannelやuser
	} else if (parsedMsg.getCommand() == "MODE") {
		if (info->findUser(parsedMsg.getParams()[0].getValue()) != info->getUsers().end()) {
			return (cmdUserMode(user, parsedMsg, info));
		}
		return (cmdChannelMode(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "PRIVMSG") {
		return (cmdPrivmsg(user, parsedMsg, info));
	} else if (parsedMsg.getCommand() == "NOTICE") {
		return (cmdNotice(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "PONG") {
	// 	return (pong(user, parsedMsg, info));
	// } else if (parsedMsg.getCommand() == "ERROR") {
	// 	return (error(user, parsedMsg, info));
	}
	return (Reply::errUnknownCommand(kERR_UNKNOWNCOMMAND, user->getNickName(), parsedMsg.getCommand()));
}
