/*
 *    Command: PONG
 * Parameters: <server> [ <server2> ]
 *
 * Numeric Replies:
 *             409 ERR_NOORIGIN     ":No origin specified"
 *             402 ERR_NOSUCHSERVER "<server name> :No such server"
 *
 * Example:
 *    PONG csd.bu.edu tolsun.oulu.fi  ; PONG message from csd.bu.edu to tolsun.oulu.fi
 *
 */

#include <iostream>
#include "../Execute.hpp"
#include "../../user/User.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"
#include "../../parser/Parser.hpp"
#include "../../debug/debug.hpp"

void	Execute::cmdPong(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		if (parsedMsg.getParams()[0].getValue() != info->getServerName()) {
			std::string reply = Reply::rplFromName(info->getServerName());
			reply += Reply::errNoOrigin(kERR_NOORIGIN, info->getServerName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		std::string	message = ":" + info->getServerName() + " PONG " + info->getServerName() + Reply::getDelimiter();
		Server::sendNonBlocking(user->getFd(), message.c_str(), message.size());
	} catch (const std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
