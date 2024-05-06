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

std::string	Execute::cmdInvite(User* user, const ParsedMessage& parsedMsg, Info* info) {
	try {
		// validation
		if (parsedMsg.getParams().size() < 2) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		std::vector<Channel>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[1].getValue());
		// Channelが存在するか確認
		if (channelIt == info->getChannels().end()) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// UserがChannel Operatorか確認
		std::vector<User*>::const_iterator	operIt = channelIt->getOperators().begin();
		for (; operIt != channelIt->getOperators().end(); operIt++) {
			if ((*operIt)->getNickName() == user->getNickName()) {
				break;
			}
		}
		if (operIt == channelIt->getOperators().end()) {
			return (Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getNickName(), user->getNickName(), parsedMsg.getParams()[1].getValue()));
		}
		// UserがChannelに居るか確認
		std::vector<User*>::const_iterator	memberIt = channelIt->getMembers().begin();
		for (; memberIt != channelIt->getMembers().end(); memberIt++) {
			if ((*memberIt)->getNickName() == user->getNickName()) {
				break;
			}
		}
		if (memberIt == channelIt->getMembers().end()) {
			return (Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getNickName(), parsedMsg.getParams()[1].getValue()));
		}
		// Target Userが存在するか確認
		std::vector<User>::iterator	targetUserIt = const_cast<std::vector<User> &>(info->getUsers()).begin();
		for (; targetUserIt != info->getUsers().end(); targetUserIt++) {
			if (parsedMsg.getParams()[0].getValue() == targetUserIt->getNickName()) {
				break;
			}
		}
		if (targetUserIt == info->getUsers().end()) {
			return (Reply::errNoSuchNick(kERR_NOSUCHNICK, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// Target Userが既にChannelに居るか確認
		for (std::vector<User*>::const_iterator	targetMemberIt = channelIt->getMembers().begin(); targetMemberIt != channelIt->getMembers().end(); targetMemberIt++) {
			if ((*targetMemberIt)->getNickName() == targetUserIt->getNickName()) {
				return (Reply::errUserOnChannel(kERR_USERONCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue(), parsedMsg.getParams()[1].getValue()));
			}
		}
		// channelIt->addMember(&(const_cast<User &>(info->getUser(targetUserIt->getIndex() - 1))));
		channelIt->addInvited(&(*targetUserIt));
		std::string	msg = ":" + user->getNickName() + " INVITE " + targetUserIt->getNickName() + " " + channelIt->getName() + "\r\n";
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(targetUserIt->getFd(), msg.c_str(), msg.size());
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		return (Reply::rplInviting(kRPL_INVITING, user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		throw;
		// return ("");
	}
}
