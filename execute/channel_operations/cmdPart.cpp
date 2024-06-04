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

std::string	Execute::cmdPart(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		if (parsedMsg.getParams().size() == 0) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		// <channel>が存在するか確認
		std::vector<Channel*>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channelIt == info->getChannels().end()) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// userが<channel>にいるか確認
		if (!(*channelIt)->isMember(user->getNickName())) {
			return (Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		(*channelIt)->eraseMember(user);
		(*channelIt)->eraseOperator(user);
		std::string	msg = ":" + user->getNickName() + " PART " + (*channelIt)->getName() + " :";
		if (parsedMsg.getParams().size() > 1) {
			msg += parsedMsg.getParams()[1].getValue() + "\r\n";
		} else {
			msg += user->getNickName() + "\r\n";
		}
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		for (std::vector<User *>::const_iterator memberIt = (*channelIt)->getMembers().begin(); memberIt != (*channelIt)->getMembers().end(); memberIt++) {
			sendNonBlocking((*memberIt)->getFd(), msg.c_str(), msg.size());
		}
		if ((*channelIt)->getMembers().size() == 0) {
			info->eraseChannel(channelIt);
		}
		return ("");
	} catch (std::exception& e) {
		throw;
	}
}
