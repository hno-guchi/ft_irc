/*
 *    Command: PART
 * Parameters: <channel> *( "," <channel> ) [ <Part Message> ]
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS  "<command> :Not enough parameters"
 *             403 ERR_NOSUCHCHANNEL   "<channel name> :No such channel"
 *             442 ERR_NOTONCHANNEL    "<channel> :You're not on that channel"
 *
 * Examples:
 *    PART #twilight_zone             ; Command to leave channel "#twilight_zone"
 *    PART #oz-ops,&group5            ; Command to leave both channels "&group5" and "#oz-ops".
 *    :WiZ!jto@tolsun.oulu.fi PART #playzone :I lost
 *                                    ; User WiZ leaving channel "#playzone" with the message "I lost".
 */

#include <vector>
#include "../Execute.hpp"
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

void	Execute::cmdPart(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());
		// <channel>が存在するか確認
		std::vector<Channel*>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channelIt == info->getChannels().end()) {
			reply += Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// userが<channel>にいるか確認
		if (!(*channelIt)->isMember(user->getNickName())) {
			reply += Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		(*channelIt)->eraseMember(user);
		(*channelIt)->eraseOperator(user);
		std::string	message = ":" + user->getPrefixName() + " PART " + (*channelIt)->getName();
		if (parsedMsg.getParams().size() > 1) {
			message += " " + parsedMsg.getParams()[1].getValue();
		}
		message += Reply::getDelimiter();
		Server::sendNonBlocking(user->getFd(), message.c_str(), message.size());
		for (std::vector<User *>::const_iterator memberIt = (*channelIt)->getMembers().begin(); memberIt != (*channelIt)->getMembers().end(); memberIt++) {
			Server::sendNonBlocking((*memberIt)->getFd(), message.c_str(), message.size());
		}
		if ((*channelIt)->getMembers().size() == 0) {
			info->eraseChannel(channelIt);
		}
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
