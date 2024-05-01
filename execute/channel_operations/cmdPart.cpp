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
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdPart(User* user, const ParsedMessage& parsedMsg, Info* info) {
	if (parsedMsg.getParams().size() == 0) {
		return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
	}
	// TODO(hnoguchi): std::vector<channel>::const_iterator	Info::getChannel(const std::string& name);を実装する？
	std::vector<Channel>::iterator	channelsIt = const_cast<std::vector<Channel> &>(info->getChannels()).begin();
	for (; channelsIt != info->getChannels().end(); channelsIt++) {
		if (channelsIt->getName() == parsedMsg.getParams()[0].getValue()) {
			break;
		}
	}
	if (channelsIt == info->getChannels().end()) {
		return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
	}
	// TODO(hnoguchi): std::vector<channel>::const_iterator	Channel::isOnMember(const std::string& nickName);を実装する？
	std::vector<User *>::iterator	userIt = const_cast<std::vector<User*>&>(channelsIt->getMembers()).begin();
	for (; userIt != channelsIt->getMembers().end(); channelsIt++) {
		if ((*userIt)->getNickName() == user->getNickName()) {
			break;
		}
	}
	if (userIt == channelsIt->getMembers().end()) {
		return (Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
	}
	// TODO(hnoguchi): Infoクラスに指定のチャンネルから指定のユーザを削除する関数を実装すること
	channelsIt->eraseMember(user);
	channelsIt->eraseOperator(user);
	std::string	msg = ":" + user->getNickName() + " PART " + channelsIt->getName() + " :";
	if (parsedMsg.getParams().size() == 1) {
		msg += user->getNickName() + "\r\n";
	} else {
		msg += parsedMsg.getParams()[1].getValue() + "\r\n";
	}
	debugPrintSendMessage("SendMsg", msg);
	sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
	return ("");
}
