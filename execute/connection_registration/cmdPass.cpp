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

int	Execute::cmdPass(User* user, const ParsedMessage& parsedMsg, Info* info) {
	if (parsedMsg.getParams().size() < 1) {
		return (kERR_NEEDMOREPARAMS);
	}
	if (user->getRegistered() & kPassCommand) {
		return (kERR_ALREADYREGISTRED);
	}
	// value check
	if (parsedMsg.getParams()[0].getValue() != info->getConfig().getPassword()) {
		return (kERR_PASSWDMISMATCH);
	}
	return (0);
}
