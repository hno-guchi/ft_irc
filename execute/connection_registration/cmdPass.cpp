/*
 * Command   : PASS
 * Parameters: <password>
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
 *             462 ERR_ALREADYREGISTRED ":Unauthorized command (already registered)"
 *
 * example   :　PASS secretpasswordhere
 */

#include <vector>
#include "../Execute.hpp"
#include "../../user/User.hpp"
#include "../../debug/debug.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

void	Execute::cmdPass(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());
		if (user->getRegistered() & kPassCommand) {
			reply += Reply::errAlreadyRegistered(kERR_ALREADYREGISTRED, user->getNickName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			throw std::invalid_argument("cmdPass");
		}
		if (parsedMsg.getParams()[0].getValue() != info->getConnectPwd()) {
			reply += Reply::errPasswordMisMatch(kERR_PASSWDMISMATCH, user->getNickName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			throw std::invalid_argument("cmdPass");
		}
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
