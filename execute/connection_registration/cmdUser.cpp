/*
 * Command   : USER
 * Parameters: <user> <mode> <unused> <realname>				// RFC 2812
 *             <username> <hostname> <servername> <realname>	// RFC 1459
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
 *             462 ERR_ALREADYREGISTRED ":Unauthorized command (already registered)"
 *
 * Example:
 *    USER guest 0 * :Ronnie Reagan
 *             ; User registering themselves with a username of "guest" and real name "Ronnie Reagan".
 *
 *    USER guest 8 * :Ronnie Reagan
 *             ; User registering themselves with a username of "guest" and real name "Ronnie Reagan", and asking to be set invisible.
 */

#include <vector>
#include "../Execute.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"
#include "../../debug/debug.hpp"

void	Execute::cmdUser(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());

		if (user->getRegistered() & kUserCommand) {
			reply += Reply::errAlreadyRegistered(kERR_ALREADYREGISTRED, user->getPrefixName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			throw std::invalid_argument("cmdUser");
		}
		user->setUserName(parsedMsg.getParams()[0].getValue());
		user->setHostName(parsedMsg.getParams()[1].getValue());
		user->setServerName(parsedMsg.getParams()[2].getValue());
		user->setRealName(parsedMsg.getParams()[3].getValue());
	} catch (const std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
