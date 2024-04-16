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

int	Execute::cmdOper(User* user, const ParsedMessage& parsedMsg, Info* info) {
	(void)info;
	if (parsedMsg.getParams().size() < 2) {
		return (kERR_NEEDMOREPARAMS);
	}
	if (user->getNickName() != parsedMsg.getParams()[0].getValue()) {
		return (0);
	}
	if (info->getConfig().getPassword() != parsedMsg.getParams()[1].getValue()) {
		return (kERR_PASSWDMISMATCH);
	}
	try {
		user->setMode(kOperator);
		std::string	msg = ":" + user->getNickName() + " MODE " + user->getNickName() + " :";
		msg +=  "+o\r\n";
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		// TODO(hnoguchi): 戻り値の確認
		return (kRPL_YOUREOPER);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (-1);
	}
}

