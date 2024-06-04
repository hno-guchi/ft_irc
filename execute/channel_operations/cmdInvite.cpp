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
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdInvite(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		// validation
		if (parsedMsg.getParams().size() < 2) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		// Channelが存在するか確認
		std::vector<Channel*>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[1].getValue());
		if (channelIt == info->getChannels().end()) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// UserがChannel Operatorか確認
		if (!(*channelIt)->isOperator(user->getNickName())) {
			return (Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getNickName(), user->getNickName(), parsedMsg.getParams()[1].getValue()));
		}
		// UserがChannelに居るか確認
		if (!(*channelIt)->isMember(user->getNickName())) {
			return (Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getNickName(), parsedMsg.getParams()[1].getValue()));
		}
		// Target Userが存在するか確認
		std::vector<User*>::iterator	targetUserIt = info->findUser(parsedMsg.getParams()[0].getValue());
		if (targetUserIt == info->getUsers().end()) {
			return (Reply::errNoSuchNick(kERR_NOSUCHNICK, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// Target Userが既にChannelに居るか確認
		if ((*channelIt)->isMember((*targetUserIt)->getNickName())) {
			return (Reply::errUserOnChannel(kERR_USERONCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue(), parsedMsg.getParams()[1].getValue()));
		}
		(*channelIt)->pushBackInvited(*info->findUser((*targetUserIt)->getNickName()));
		std::string	msg = ":" + user->getNickName() + " INVITE " + (*targetUserIt)->getNickName() + " " + (*channelIt)->getName() + "\r\n";
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking((*targetUserIt)->getFd(), msg.c_str(), msg.size());
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		return (Reply::rplInviting(kRPL_INVITING, user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		throw;
		// return ("");
	}
}
