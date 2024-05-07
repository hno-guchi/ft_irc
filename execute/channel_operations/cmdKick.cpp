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

std::string	Execute::cmdKick(User* user, const ParsedMessage& parsedMsg, Info* info) {
	try {
		if (parsedMsg.getParams().size() < 2) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		// <channel>が存在するか確認
		std::vector<Channel*>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channelIt == info->getChannels().end()) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// <user>が存在するか確認
		std::vector<User*>::iterator	targetUserIt = info->findUser(parsedMsg.getParams()[1].getValue());
		if (targetUserIt == info->getUsers().end()) {
			return (Reply::errNoSuchNick(kERR_NOSUCHNICK, user->getNickName(), parsedMsg.getParams()[1].getValue()));
		}
		// userがchannel operatorか確認
		if (!(*channelIt)->isOperator(user->getNickName())) {
			return (Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getNickName(), user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// <user>が<channel>にいるか確認
		if (!(*channelIt)->isMember(parsedMsg.getParams()[1].getValue())) {
			return (Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
		}
		// userが<channel>にいるか確認
		if (!(*channelIt)->isMember(user->getNickName())) {
			return (Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// 実行処理
		// <channel>から<user>を削除
		(*channelIt)->eraseMember(*targetUserIt);
		(*channelIt)->eraseOperator(*targetUserIt);
		// <user>にPARTメッセージを送信
		std::string	msg = ":" + (*targetUserIt)->getNickName() + " PART " + (*channelIt)->getName();
		if (parsedMsg.getParams().size() > 2) {
			msg += " :" + parsedMsg.getParams()[2].getValue() + "\r\n";
		} else {
			msg += " :" + parsedMsg.getParams()[1].getValue() + "\r\n";
		}
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking((*targetUserIt)->getFd(), msg.c_str(), msg.size());
		msg = ":" + user->getNickName() + " KICK " + (*channelIt)->getName() + " " + parsedMsg.getParams()[1].getValue();
		if (parsedMsg.getParams().size() > 2) {
			msg += " :" + parsedMsg.getParams()[2].getValue();
		}
		msg += "\r\n";
		debugPrintSendMessage("SendMsg", msg);
		for (std::vector<User*>::const_iterator memberIt = (*channelIt)->getMembers().begin(); memberIt != (*channelIt)->getMembers().end(); memberIt++) {
			sendNonBlocking((*memberIt)->getFd(), msg.c_str(), msg.size());
		}
		return ("");
	} catch (const std::exception& e) {
		fatalError(e.what());
		throw;
		// return ("");
	}
}
