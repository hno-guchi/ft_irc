/*
 *
 *    Command: TOPIC
 * Parameters: <channel> [ <topic> ]
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS    "<command> :Not enough parameters"
 *             442 ERR_NOTONCHANNEL      "<channel> :You're not on that channel"
 *             331 RPL_NOTOPIC           "<channel> :No topic is set"
 *             332 RPL_TOPIC             "<channel> :<topic>"
 *             482 ERR_CHANOPRIVSNEEDED  "<channel> :You're not channel operator"
 *             477 ERR_NOCHANMODES       "<channel> :Channel doesn't support modes"
 *
 * Examples:
 *    :WiZ!jto@tolsun.oulu.fi TOPIC #test :New topic
 *                                    ; User Wiz setting the topic.
 *    TOPIC #test :another topic      ; Command to set the topic on #test to "another topic".
 *    TOPIC #test :                   ; Command to clear the topic on #test.
 *    TOPIC #test                     ; Command to check the topic for #test.
 */

#include <vector>
#include "../Execute.hpp"
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdTopic(User* user, const ParsedMessage& parsedMsg, Info* info) {
	try {
		if (parsedMsg.getParams().size() < 1) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		// <channel>が存在するか確認
		std::vector<Channel>::iterator	channelIt = const_cast<std::vector<Channel> &>(info->getChannels()).begin();
		for (; channelIt != info->getChannels().end(); channelIt++) {
			if (channelIt->getName() == parsedMsg.getParams()[0].getValue()) {
				break;
			}
		}
		if (channelIt == info->getChannels().end()) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// userがchannel operatorか確認
		std::vector<User*>::const_iterator	operIt = channelIt->getOperators().begin();
		for (; operIt != channelIt->getOperators().end(); operIt++) {
			if ((*operIt)->getNickName() == user->getNickName()) {
				break;
			}
		}
		if (operIt == channelIt->getOperators().end()) {
			return (Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getNickName(), user->getNickName(), parsedMsg.getParams()[1].getValue()));
		}
		// <topic>がない場合、トピックを返す
		if (parsedMsg.getParams().size() == 1) {
			if (channelIt->getTopic().empty()) {
				return (Reply::rplNoTopic(kRPL_NOTOPIC, user->getNickName(), channelIt->getName()));
			}
			return (Reply::rplTopic(kRPL_TOPIC, user->getNickName(), channelIt->getName(), channelIt->getTopic()));
		}
		// topicの設定処理
		// userが<channel>に参加しているか確認
		std::vector<User*>::const_iterator	userIt = channelIt->getMembers().begin();
		for (; userIt != channelIt->getMembers().end(); userIt++) {
			if ((*userIt)->getNickName() == user->getNickName()) {
				break;
			}
		}
		if (userIt == channelIt->getMembers().end()) {
			return (Reply::errNotOnChannel(kERR_NOTONCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// <channel>にt modeが設定されているか確認
		if (channelIt->getModes() & kRestrictTopicSetting) {
			return (Reply::errNoChanModes(kERR_NOCHANMODES, user->getNickName(), user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// topicの設定を実行
		channelIt->setTopic(parsedMsg.getParams()[1].getValue());
		// <channel>のメンバにtopicの変更を通知
		std::string	msg = ":" + user->getNickName() + " TOPIC " + channelIt->getName() + " :" + channelIt->getTopic() + "\r\n";
		debugPrintSendMessage("SendMsg", msg);
		for (std::vector<User*>::const_iterator	memberIt = channelIt->getMembers().begin(); memberIt != channelIt->getMembers().end(); memberIt++) {
			sendNonBlocking((*memberIt)->getFd(), msg.c_str(), msg.size());
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return ("");
	}
	return ("");
}
