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
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdQuit(User* user, const ParsedMsg& parsedMsg, Info* info) {
	std::string	message = ":" + user->getNickName() + " ERROR :";
	if (parsedMsg.getParams().size() > 0) {
		message += parsedMsg.getParams()[0].getValue();
	} else {
		message += "Client Quit";
	}
	message += "\r\n";
	sendNonBlocking(user->getFd(), message.c_str(), message.size());
	info->eraseUser(info->findUser(user->getFd()));
	return ("");
}
