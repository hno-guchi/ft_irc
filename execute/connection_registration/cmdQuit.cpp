/*
 *    Command: QUIT
 * Parameters: [ <Quit Message> ]
 *
 * Numeric Replies:
 *             None.
 *
 * Example:
 *    QUIT :Gone to have lunch
 *             ; Preferred message format.
 *
 *    :syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch
 *             ; User syrk has quit IRC to have lunch.
 */

#include <vector>
#include "../Execute.hpp"
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

void	Execute::cmdQuit(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	message = ":" + user->getPrefixName() + " ERROR :";
		if (parsedMsg.getParams().size() > 0) {
			message += parsedMsg.getParams()[0].getValue();
		} else {
			message += "Client Quit";
		}
		message += Reply::getDelimiter();
		Server::sendNonBlocking(user->getFd(), message.c_str(), message.size());
		info->eraseUser(info->findUser(user->getFd()));
	} catch (const std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
