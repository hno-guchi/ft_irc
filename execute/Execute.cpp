#include "./Execute.hpp"
#include "../server/Server.hpp"
#include "../server/Info.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"
#include "../debug/debug.hpp"
#include "../parser/Parser.hpp"
#include "../parser/ParsedMsg.hpp"

// CONSTRUCTOR & DESTRUCTOR
Execute::Execute() {}
Execute::~Execute() {}

void	Execute::registerUser(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());
		if (!(user->getRegistered() & kPassCommand) && parsedMsg.getCommand() == "PASS") {
			this->cmdPass(user, parsedMsg, info);
			user->setRegistered(kPassCommand);
			return;
		}
		if (!(user->getRegistered() & kNickCommand) && parsedMsg.getCommand() == "NICK") {
			if (user->getRegistered() & kPassCommand) {
				this->cmdNick(user, parsedMsg, info);
				user->setRegistered(kNickCommand);
				return;
			}
			reply += Reply::errPasswordMisMatch(kERR_PASSWDMISMATCH, "*");
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			throw std::invalid_argument("registerUser");
		}
		if (!(user->getRegistered() & kUserCommand) && parsedMsg.getCommand() == "USER") {
			if ((user->getRegistered() & kPassCommand) && (user->getRegistered() & kNickCommand)) {
				this->cmdUser(user, parsedMsg, info);
				user->setRegistered(kUserCommand);
				reply += Reply::rplWelcome(*info, *user);
				Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
				return;
			}
			if (!(user->getRegistered() & kPassCommand)) {
				reply += Reply::errPasswordMisMatch(kERR_PASSWDMISMATCH, "*");
			} else {
				reply += Reply::errNoNickNameGiven(kERR_NONICKNAMEGIVEN, "*");
			}
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			throw std::invalid_argument("registerUser");
		}
	} catch (std::exception& e) {
		throw;
	}
}

void	Execute::exec(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		if (parsedMsg.getCommand() == "PING") {
			this->cmdPong(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "NICK") {
			this->cmdNick(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "OPER") {
			this->cmdOper(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "QUIT") {
			this->cmdQuit(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "JOIN") {
			this->cmdJoin(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "PART") {
			this->cmdPart(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "KICK") {
			this->cmdKick(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "INVITE") {
			this->cmdInvite(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "TOPIC") {
			this->cmdTopic(user, parsedMsg, info);
		// TODO(hnoguchi): Paramsのtypeにchannelやuser
		} else if (parsedMsg.getCommand() == "MODE") {
			if (info->findUser(parsedMsg.getParams()[0].getValue()) != info->getUsers().end()) {
				this->cmdUserMode(user, parsedMsg, info);
			}
			this->cmdChannelMode(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "PRIVMSG") {
			this->cmdPrivmsg(user, parsedMsg, info);
		} else if (parsedMsg.getCommand() == "NOTICE") {
			this->cmdNotice(user, parsedMsg, info);
		}
	} catch (std::exception& e) {
		throw;
	}
}
