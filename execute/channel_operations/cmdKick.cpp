/*
 *    Command: KICK
 * Parameters: <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS    "<command> :Not enough parameters"
 *             403 ERR_NOSUCHCHANNEL     "<channel name> :No such channel"
 *             476 ERR_BADCHANMASK       "<channel> :Bad Channel Mask"
 *             482 ERR_CHANOPRIVSNEEDED  "<channel> :You're not channel operator"
 *             441 ERR_USERNOTINCHANNEL  "<nick> <channel> :They aren't on that channel"
 *             442 ERR_NOTONCHANNEL      "<channel> :You're not on that channel"
 * Examples:
 *    KICK &Melbourne Matthew         ; Command to kick Matthew from &Melbourne
 *    KICK #Finnish John :Speaking English
 *                                    ; Command to kick John from #Finnish using "Speaking English" as the reason (comment).
 *    :WiZ!jto@tolsun.oulu.fi KICK #Finnish John
 *                                    ; KICK message on channel #Finnish from WiZ to remove John from channel
 *
 */

#include <vector>
#include "../Execute.hpp"
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"

int	Execute::cmdKick(User* user, const ParsedMessage& parsedMsg, Info* info) {
	if (parsedMsg.getParams().size() < 2) {
		return (kERR_NEEDMOREPARAMS);
	}
	std::vector<Channel>::iterator	channelIt = const_cast<std::vector<Channel> &>(info->getChannels()).begin();
	for (; channelIt != info->getChannels().end(); channelIt++) {
		if (channelIt->getName() == parsedMsg.getParams()[0]) {
			break;
		}
	}
	if (channelIt == info->getChannels().end()) {
		return (kERR_NOSUCHCHANNEL);
	}
	std::vector<User*>::const_iterator	operIt = channelIt->getOerators().begin();
	for (; operIt != channelIt->getOperators().end(); operIt++) {
		if ((*operIt)->getNick() == user->getNick()) {
			break;
		}
	}
	if (operIt == channelIt->getOperators().end()) {
		return (kERR_CHANOPRIVSNEEDED);
	}

}
