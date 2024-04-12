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
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

int	Execute::cmdUser(User* user, const ParsedMessage& parsedMsg, Info* info) {
	(void)info;
	// TODO(hnoguchi): Parser classでバリデーションを行う。
	if (parsedMsg.getParams().size() < 4) {
		return (kERR_NEEDMOREPARAMS);
	}
	if (user->getRegistered() & kUserCommand) {
		return (kERR_ALREADYREGISTRED);
	}
	try {
		user->setUserName(parsedMsg.getParams()[0].getValue());
		user->setHostName(parsedMsg.getParams()[1].getValue());
		user->setServerName(parsedMsg.getParams()[2].getValue());
		user->setRealName(parsedMsg.getParams()[3].getValue());
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		// TODO(hnoguchi): 適切なエラーナンバーを返す。
		return (-1);
	}
	return (0);
}
