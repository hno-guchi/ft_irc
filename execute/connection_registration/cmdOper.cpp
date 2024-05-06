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
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdOper(User* user, const ParsedMessage& parsedMsg, Info* info) {
	(void)info;
	try {
		if (parsedMsg.getParams().size() < 2) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		if (user->getNickName() != parsedMsg.getParams()[0].getValue()) {
			return (Reply::errUsersDontMatch(kERR_USERSDONTMATCH, user->getNickName()));
		}
		if (info->getConfig().getPassword() != parsedMsg.getParams()[1].getValue()) {
			return (Reply::errPasswordMisMatch(kERR_PASSWDMISMATCH, user->getNickName()));
		}
		user->setMode(kOperator);
		std::string	msg = ":" + user->getNickName() + " MODE " + user->getNickName() + " :+o\r\n";
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		return (Reply::rplYourOper(kRPL_YOUREOPER, user->getNickName(), user->getNickName()));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return ("");
	}
}
