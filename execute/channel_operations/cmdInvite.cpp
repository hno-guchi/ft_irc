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

void	Execute::cmdInvite(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());

		// Channelが存在するか確認
		Channel*	channel = info->findChannel(parsedMsg.getParams()[1].getValue());
		if (channel == NULL) {
			reply += Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getPrefixName(), parsedMsg.getParams()[1].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// UserがChannel Operatorか確認
		if (!channel->isOperator(user)) {
			reply += Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getPrefixName(), parsedMsg.getParams()[1].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// UserがChannelに居るか確認
		if (!channel->isMember(user)) {
			reply += Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getPrefixName(), parsedMsg.getParams()[1].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// Target Userが存在するか確認
		User*	targetUser = info->findUser(parsedMsg.getParams()[0].getValue());
		if (targetUser == NULL) {
			reply += Reply::errNoSuchNick(kERR_NOSUCHNICK, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// Target Userが既にChannelに居るか確認
		if (channel->isMember(targetUser)) {
			reply += Reply::errUserOnChannel(kERR_USERONCHANNEL, user->getPrefixName(), targetUser->getNickName(), parsedMsg.getParams()[1].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		if (!channel->isInvited(targetUser)) {
			channel->pushBackInvited(targetUser);
		}
		std::string	msg = ":" + user->getPrefixName() + " INVITE " + targetUser->getNickName() + " " + channel->getName() + Reply::getDelimiter();
		Server::sendNonBlocking(targetUser->getFd(), msg.c_str(), msg.size());
		Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		reply += Reply::rplInviting(kRPL_INVITING, user->getPrefixName(), targetUser->getNickName(), channel->getName());
		Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
