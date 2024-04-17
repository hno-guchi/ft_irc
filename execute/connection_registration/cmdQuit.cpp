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

int	Execute::cmdQuit(User* user, const ParsedMessage& parsedMsg, Info* info) {
	(void)parsedMsg;
	info->eraseUser(user);
	return (-1);
}
