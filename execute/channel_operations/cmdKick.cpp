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
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"

void	Execute::cmdKick(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());
		// <channel>が存在するか確認
		Channel*	channel = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channel == NULL) {
			reply += Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// <user>が存在するか確認
		User*	targetUser = info->findUser(parsedMsg.getParams()[1].getValue());
		if (targetUser == NULL) {
			reply += Reply::errNoSuchNick(kERR_NOSUCHNICK, user->getPrefixName(), parsedMsg.getParams()[1].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// userがchannel operatorか確認
		if (!channel->isOperator(user)) {
			reply += Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// <user>が<channel>にいるか確認
		if (!channel->isMember(targetUser)) {
			reply += Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getPrefixName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// userが<channel>にいるか確認
		if (!channel->isMember(user)) {
			reply += Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// 実行処理
		// <channel>から<user>を削除
		info->eraseUserInChannel(targetUser, channel);
		// <user>にPARTメッセージを送信
		std::string	msg = ":" + targetUser->getPrefixName() + " PART " + channel->getName();
		if (parsedMsg.getParams().size() > 2) {
			msg += " " + parsedMsg.getParams()[2].getValue();
		}
		msg += Reply::getDelimiter();
		Server::sendNonBlocking(targetUser->getFd(), msg.c_str(), msg.size());
		msg = ":" + user->getPrefixName() + " KICK " + channel->getName() + " " + parsedMsg.getParams()[1].getValue();
		if (parsedMsg.getParams().size() > 2) {
			msg += " " + parsedMsg.getParams()[2].getValue();
		}
		msg += Reply::getDelimiter();
		for (std::vector<User*>::const_iterator memberIt = channel->getMembers().begin(); memberIt != channel->getMembers().end(); memberIt++) {
			Server::sendNonBlocking((*memberIt)->getFd(), msg.c_str(), msg.size());
		}
	} catch (const std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
