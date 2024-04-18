/*
 * Command   : MODE
 * Parameters: <nickname> *( ( "+" / "-" ) *( "i" / "w" / "o" / "O" / "r" ) )
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
 *             502 ERR_USERSDONTMATCH   ":Cannot change mode for other users"
 *             501 ERR_UMODEUNKNOWNFLAG ":Unknown MODE flag"
 *             221 RPL_UMODEIS          "<user mode string>"
 *
 * Examples:
 *    MODE WiZ -w
 *             ; Command by WiZ to turn off reception of WALLOPS messages.
 *
 *    MODE Angel +i
 *             ; Command from Angel to make herself invisible.
 *
 *    MODE WiZ -o
 *             ; WiZ 'deopping' (removing operator status).
 */

#include <vector>
#include "../Execute.hpp"
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

int	Execute::cmdUserMode(User* user, const ParsedMessage& parsedMsg, Info* info) {
	(void)info;
	if (parsedMsg.getParams().size() < 1) {
		return (kERR_NEEDMOREPARAMS);
	}
	if (user->getNickName() != parsedMsg.getParams()[0].getValue()) {
		return (kERR_USERSDONTMATCH);
	}
	if (parsedMsg.getParams().size() == 1) {
		return (kRPL_UMODEIS);
	}
	if (parsedMsg.getParams()[1].size() != 2) {
		return (kERR_UMODEUNKNOWNFLAG);
	}
	if (parsedMsg.getParams()[1].getValue()[0] != '+' && parsedMsg.getParams()[1].getValue()[0] != '-') {
		return (kERR_UMODEUNKNOWNFLAG);
	}
	// TODO(hnoguchi): "+oOr"のような文字列はParser classで"+o", "+O", "+r"に分割する。
	if (parsedMsg.getParams()[1].getValue()[0] == '-') {
		if (parsedMsg.getParams()[1].getValue()[1] != 'o') {
			return (kERR_UMODEUNKNOWNFLAG);
		}
		user->unsetMode(kOperator);
		std::string	msg = ":" + user->getNickName() + " MODE " + user->getNickName() + " :";
		msg +=  "-o\r\n";
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		// TODO(hnoguchi): 戻り値の確認
		return (0);
	}
	if (parsedMsg.getParams()[1].getValue()[0] == '+') {
		if (parsedMsg.getParams()[1].getValue()[1] != 'r') {
			return (kERR_UMODEUNKNOWNFLAG);
		}
		user->unsetMode(kOperator);
		user->setMode(kRestrict);
		try {
			std::string	msg = ":" + user->getNickName() + " MODE " + user->getNickName() + " :";
			msg +=  "+r\r\n";
			msg += ":" + user->getNickName() + " MODE " + user->getNickName() + " :";
			msg +=  "-o\r\n";
			debugPrintSendMessage("SendMsg", msg);
			sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			// TODO(hnoguchi): 戻り値の確認
			return (0);
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			return (-1);
		}
	}
	return (kERR_UMODEUNKNOWNFLAG);
}
