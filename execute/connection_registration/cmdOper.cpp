/*
 *    Command: OPER
 * Parameters: <name> <password>
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
 *             381 RPL_YOUREOPER        ":You are now an IRC operator"
 *             [491 ERR_NOOPERHOST       ":No O-lines for your host"]
 *             464 ERR_PASSWDMISMATCH   ":Password incorrect"
 * Example:
 *    OPER foo bar
 *             ; Attempt to register as an operator using a username of "foo" and "bar" as the password.
 */

#include <vector>
#include "../Execute.hpp"
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

void	Execute::cmdOper(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());

		if (user->getNickName() != parsedMsg.getParams()[0].getValue()) {
			reply += Reply::errUsersDontMatch(kERR_USERSDONTMATCH, user->getPrefixName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		if (info->getOperPwd() != parsedMsg.getParams()[1].getValue()) {
			reply += Reply::errPasswordMisMatch(kERR_PASSWDMISMATCH, user->getPrefixName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		user->setMode(kOperator);
		std::string	msg = ":" + user->getPrefixName() + " MODE " + user->getNickName() + " :+o\r\n";
		Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		reply += Reply::rplYourOper(kRPL_YOUREOPER, user->getPrefixName(), user->getNickName());
		Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
