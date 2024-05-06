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

std::string	Execute::cmdUserMode(User* user, const ParsedMessage& parsedMsg, Info* info) {
	(void)info;
	try {
		if (parsedMsg.getParams().size() < 1) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		if (user->getNickName() != parsedMsg.getParams()[0].getValue()) {
			return (Reply::errUsersDontMatch(kERR_USERSDONTMATCH, user->getNickName()));
		}
		if (parsedMsg.getParams().size() == 1) {
			return (Reply::rplUModeIs(kRPL_UMODEIS, user->getNickName(), *user));
		}
		if (parsedMsg.getParams()[1].getValue().size() != 2) {
			return (Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user->getNickName()));
		}
		if (parsedMsg.getParams()[1].getValue()[0] != '+' && parsedMsg.getParams()[1].getValue()[0] != '-') {
			return (Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user->getNickName()));
		}
		// TODO(hnoguchi): "+oOr"のような文字列はParser classで"+o", "+O", "+r"に分割する。
		if (parsedMsg.getParams()[1].getValue()[0] == '-') {
			if (parsedMsg.getParams()[1].getValue()[1] != 'o') {
				return (Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user->getNickName()));
			}
			user->unsetMode(kOperator);
			std::string	msg = ":" + user->getNickName() + " MODE " + user->getNickName() + " :-o\r\n";
			debugPrintSendMessage("SendMsg", msg);
			sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			return ("");
		}
		if (parsedMsg.getParams()[1].getValue()[0] == '+') {
			if (parsedMsg.getParams()[1].getValue()[1] != 'r') {
				return (Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user->getNickName()));
			}
			user->unsetMode(kOperator);
			user->setMode(kRestrict);
			std::string	msg = ":" + user->getNickName() + " MODE " + user->getNickName() + " :+r\r\n";
			if (user->getModes() & kOperator) {
				msg += ":" + user->getNickName() + " MODE " + user->getNickName() + " :-o\r\n";
			}
			debugPrintSendMessage("SendMsg", msg);
			sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			return ("");
		}
		return (Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user->getNickName()));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return ("");
	}
}
