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
#include "../../error/error.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdPass(User* user, const ParsedMsg& parsedMsg, Info* info) {
	(void)info;
	if (parsedMsg.getParams().size() < 1) {
		return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
	}
	if (user->getRegistered() & kPassCommand) {
		return (Reply::errAlreadyRegistered(kERR_ALREADYREGISTRED, user->getNickName()));
	}
	if (parsedMsg.getParams()[0].getValue() != info->getConfig().getPassword()) {
		return (Reply::errPasswordMisMatch(kERR_PASSWDMISMATCH, user->getNickName()));
	}
	return ("");
}
