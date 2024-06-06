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
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

void	Execute::cmdUserMode(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());

		if (user->getNickName() != parsedMsg.getParams()[0].getValue()) {
			reply += Reply::errUsersDontMatch(kERR_USERSDONTMATCH, user->getPrefixName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		if (parsedMsg.getParams().size() == 1) {
			reply += Reply::rplUModeIs(kRPL_UMODEIS, user->getPrefixName(), *user);
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		if (parsedMsg.getParams()[1].getValue().size() != 2) {
			reply += Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user->getPrefixName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		if (parsedMsg.getParams()[1].getValue()[0] == '-') {
			if (parsedMsg.getParams()[1].getValue()[1] != 'o') {
				reply += Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user->getPrefixName());
				Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
				return;
			}
			user->unsetMode(kOperator);
			std::string	msg = ":" + user->getPrefixName() + " MODE " + user->getNickName() + " :-o" + Reply::getDelimiter();
			Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			return;
		}
		reply += Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user->getPrefixName());
		Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
