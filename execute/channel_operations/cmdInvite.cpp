/*
 *    Command: INVITE
 * Parameters: <nickname> <channel>
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS    "<command> :Not enough parameters"
 *             401 ERR_NOSUCHNICK        "<nickname> :No such nick/channel"
 *             442 ERR_NOTONCHANNEL      "<channel> :You're not on that channel"
 *             443 ERR_USERONCHANNEL     "<user> <channel> :is already on channel"
 *             482 ERR_CHANOPRIVSNEEDED  "<channel> :You're not channel operator"
 *             341 RPL_INVITING          "<channel> <nick>"
 *             301 RPL_AWAY              "<nick> :<away message>"
 * Examples:
 *    :Angel!wings@irc.org INVITE Wiz #Dust
 *                                    ; Message to WiZ when he has been invited by user Angel to channel #Dust
 *    INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to #Twilight_zone
 */

#include <vector>
#include "../Execute.hpp"
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"

int	Execute::cmdInvite(User* user, const ParsedMessage& parsedMsg, Info* info) {
	// validation
	if (parsedMsg.getParams().size() < 2) {
		return (kERR_NEEDMOREPARAMS);
	}
	std::vector<User>::iterator	targetUserIt = const_cast<std::vector<User> &>(info->getUsers()).begin();
	for (; targetUserIt != info->getUsers().end(); targetUserIt++) {
		if (parsedMsg.getParams()[0].getValue() == targetUserIt->getNickName()) {
			break;
		}
	}
	if (targetUserIt == info->getUsers().end()) {
		return (kERR_NOSUCHNICK);
	}
	std::vector<Channel>::iterator	channelIt = const_cast<std::vector<Channel> &>(info->getChannels()).begin();
	for (; channelIt != info->getChannels().end(); channelIt++) {
		if (parsedMsg.getParams()[1].getValue() == channelIt->getName()) {
			break;
		}
	}
	if (channelIt == info->getChannels().end()) {
		return (kERR_NOSUCHNICK);
	}
	// if (channelIt != info->getChannels().end()) {
		std::vector<User*>::const_iterator	memberIt = channelIt->getMembers().begin();
		for (; memberIt != channelIt->getMembers().end(); memberIt++) {
			if ((*memberIt)->getNickName() == user->getNickName()) {
				break;
			}
		}
		if (memberIt == channelIt->getMembers().end()) {
			return (kERR_NOTONCHANNEL);
		}
		if (channelIt->getModes() & kInviteOnly) {
			std::vector<User*>::const_iterator	operIt = channelIt->getOperators().begin();
			for (; operIt != channelIt->getOperators().end(); operIt++) {
				if ((*operIt)->getNickName() == user->getNickName()) {
					break;
				}
			}
			if (operIt == channelIt->getOperators().end()) {
				std::cerr << "Not channel oper" << std::endl;
				return (kERR_CHANOPRIVSNEEDED);
			}
		}
	// }
	for (std::vector<User*>::const_iterator	memberIt = channelIt->getMembers().begin(); memberIt != channelIt->getMembers().end(); memberIt++) {
		if ((*memberIt)->getNickName() == targetUserIt->getNickName()) {
			std::cerr << "target user on already" << std::endl;
			return (kERR_USERONCHANNEL);
		}
	}
	try {
		channelIt->addMember(&(const_cast<User &>(info->getUser(targetUserIt->getIndex() - 1))));
		std::string	msg = ":" + user->getNickName() + " INVITE " + targetUserIt->getNickName() + " " + channelIt->getName() + "\r\n";
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(targetUserIt->getFd(), msg.c_str(), msg.size());
		// TODO(hnoguchi): Check error
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		// TODO(hnoguchi): Check error
		return (kRPL_INVITING);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	return (0);
}
