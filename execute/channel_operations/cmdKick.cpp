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
		std::vector<Channel*>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channelIt == info->getChannels().end()) {
			reply += Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// <user>が存在するか確認
		std::vector<User*>::iterator	targetUserIt = info->findUser(parsedMsg.getParams()[1].getValue());
		if (targetUserIt == info->getUsers().end()) {
			reply += Reply::errNoSuchNick(kERR_NOSUCHNICK, user->getPrefixName(), parsedMsg.getParams()[1].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// userがchannel operatorか確認
		if (!(*channelIt)->isOperator(user->getNickName())) {
			reply += Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getPrefixName(), user->getNickName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// <user>が<channel>にいるか確認
		if (!(*channelIt)->isMember(parsedMsg.getParams()[1].getValue())) {
			reply += Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getPrefixName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// userが<channel>にいるか確認
		if (!(*channelIt)->isMember(user->getNickName())) {
			reply += Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// 実行処理
		// <channel>から<user>を削除
		(*channelIt)->eraseMember(*targetUserIt);
		(*channelIt)->eraseOperator(*targetUserIt);
		// <user>にPARTメッセージを送信
		std::string	msg = ":" + (*targetUserIt)->getNickName() + " PART " + (*channelIt)->getName();
		if (parsedMsg.getParams().size() > 2) {
			msg += " :" + parsedMsg.getParams()[2].getValue() + Reply::getDelimiter();
		} else {
			msg += " :" + parsedMsg.getParams()[1].getValue() + Reply::getDelimiter();
		}
		Server::sendNonBlocking((*targetUserIt)->getFd(), msg.c_str(), msg.size());
		msg = ":" + user->getPrefixName() + " KICK " + (*channelIt)->getName() + " " + parsedMsg.getParams()[1].getValue();
		if (parsedMsg.getParams().size() > 2) {
			msg += " :" + parsedMsg.getParams()[2].getValue();
		}
		msg += Reply::getDelimiter();
		for (std::vector<User*>::const_iterator memberIt = (*channelIt)->getMembers().begin(); memberIt != (*channelIt)->getMembers().end(); memberIt++) {
			Server::sendNonBlocking((*memberIt)->getFd(), msg.c_str(), msg.size());
		}
	} catch (const std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
