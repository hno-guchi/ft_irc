/*
 * Command   : JOIN
 * Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS  "<command> :Not enough parameters"
 *             474 ERR_BANNEDFROMCHAN  "<channel> :Cannot join channel (+b)"
 *             473 ERR_INVITEONLYCHAN  "<channel> :Cannot join channel (+i)"
 *             475 ERR_BADCHANNELKEY   "<channel> :Cannot join channel (+k)"
 *             471 ERR_CHANNELISFULL   "<channel> :Cannot join channel (+l)"
 *             476 ERR_BADCHANMASK     "<channel> :Bad Channel Mask"
 *             403 ERR_NOSUCHCHANNEL   "<channel name> :No such channel"
 *             405 ERR_TOOMANYCHANNELS "<target> :<error code> recipients. <abort message>"
 *             407 ERR_TOOMANYTARGETS  "<target> :<error code> recipients. <abort message>"
 *             437 ERR_UNAVAILRESOURCE "<nick/channel> :Nick/channel is temporarily unavailable"
 *             332 RPL_TOPIC           "<channel> :<topic>"
 *
 * Examples:
 *    JOIN #foobar                ; Command to join channel #foobar.
 *    JOIN &foo fubar             ; Command to join channel &foo using key "fubar".
 *    JOIN #foo,&bar fubar        ; Command to join channel #foo using key "fubar" and &bar using no key.
 *    JOIN #foo,#bar fubar,foobar ; Command to join channel #foo using key "fubar", and channel #bar using key "foobar".
 *    JOIN #foo,#bar              ; Command to join channels #foo and #bar.
 *    JOIN 0                      ; Leave all currently joined channels.
 *    :WiZ!jto@tolsun.oulu.fi JOIN #Twilight_zone
 *                                ; JOIN message from WiZ on channel #Twilight_zone
 */

#include <iostream>
#include <vector>
#include "../Execute.hpp"
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdJoin(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		// Check 461 ERR_NEEDMOREPARAMS  "<command> :Not enough parameters"
		if (parsedMsg.getParams().size() < 1) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		// channelを探す
		std::vector<Channel*>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		// <channel>が存在する場合
		if (channelIt != info->getChannels().end()) {
			// 471 ERR_CHANNELISFULL   "<channel> :Cannot join channel (+l)"
			if ((*channelIt)->getModes() & kLimit) {
				if ((*channelIt)->getMembers().size() >= static_cast<unsigned long>((*channelIt)->getLimit())) {
					return (Reply::errChannelIsFull(kERR_CHANNELISFULL, user->getNickName(), (*channelIt)->getName()));
				}
			}
			// Check 473 ERR_INVITEONLYCHAN  "<channel> :Cannot join channel (+i)"
			if ((*channelIt)->getModes() & kInviteOnly) {
				if (!(*channelIt)->isInvited(user->getNickName())) {
					return (Reply::errInviteOnlyChan(kERR_INVITEONLYCHAN, user->getNickName(), (*channelIt)->getName()));
				}
				(*channelIt)->eraseInvited(*info->findUser(user->getNickName()));
			}
			// 475 ERR_BADCHANNELKEY   "<channel> :Cannot join channel (+k)"
			if ((*channelIt)->getModes() & kKey) {
					std::cerr << "[" << parsedMsg.getParams()[1].getValue() <<"] | [" << (*channelIt)->getKey() << "]" << std::endl;
				if (parsedMsg.getParams().size() < 2) {
					return (Reply::errBadChannelKey(kERR_BADCHANNELKEY, user->getNickName(), (*channelIt)->getName()));
				}
				if (parsedMsg.getParams()[1].getValue() != (*channelIt)->getKey()) {
					return (Reply::errBadChannelKey(kERR_BADCHANNELKEY, user->getNickName(), (*channelIt)->getName()));
				}
			}
			// userを<channel>に追加
			(*channelIt)->pushBackMember(user);
			std::string	msg = ":" + user->getNickName() + " JOIN " + (*channelIt)->getName() + "\r\n";
			debugPrintSendMessage("SendMsg", msg);
			(*channelIt)->printData();
			for (std::vector<User*>::const_iterator memberIt = (*channelIt)->getMembers().begin(); memberIt != (*channelIt)->getMembers().end(); memberIt++) {
				// (*memberIt)->printData();
				sendNonBlocking((*memberIt)->getFd(), msg.c_str(), msg.size());
			}
			// JOINしたuserへchannel情報(RPL_(NO)TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES)を送信
			msg = Reply::rplFromName(info->getServerName());
			if ((*channelIt)->getTopic().size() > 0) {
				msg += Reply::rplTopic(kRPL_TOPIC, user->getNickName(), (*channelIt)->getName(), (*channelIt)->getTopic());
			} else {
				msg += Reply::rplNoTopic(kRPL_NOTOPIC, user->getNickName(), (*channelIt)->getName());
			}
			debugPrintSendMessage("SendMsg", msg);
			sendNonBlocking(user->getFd(), msg.c_str(), msg.size());

			msg = Reply::rplFromName(info->getServerName());
			msg += Reply::rplNamReply(kRPL_NAMREPLY, user->getNickName(), *(*channelIt));
			msg += Reply::rplFromName(info->getServerName());
			msg += Reply::rplEndOfNames(kRPL_ENDOFNAMES, user->getNickName(), (*channelIt)->getName());
			debugPrintSendMessage("SendMsg", msg);
			sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			// if ((*channelIt)->getTopic().size() > 0) {
			// 	msg = ":" + info->getServerName() + " TOPIC " + (*channelIt)->getName() + " :" + (*channelIt)->getTopic() + "\r\n";
			// 	debugPrintSendMessage("SendMsg", msg);
			// 	sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			// }
			// if ((*channelIt)->getModes() & kInviteOnly) {
			// 	msg = ":" + info->getServerName() + " MODE " + (*channelIt)->getName() + " :+i\r\n";
			// 	debugPrintSendMessage("SendMsg", msg);
			// 	sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			// }
			// if ((*channelIt)->getModes() & kKey) {
			// 	msg = ":" + info->getServerName() + " MODE " + (*channelIt)->getName() + " :+k " + (*channelIt)->getKey() + "\r\n";
			// 	debugPrintSendMessage("SendMsg", msg);
			// 	sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			// }
			// if ((*channelIt)->getModes() & kLimit) {
			// 	msg += ":" + info->getServerName() + " MODE " + (*channelIt)->getName() + " :+l " + std::to_string((*channelIt)->getLimit()) + "\r\n";
			// 	debugPrintSendMessage("SendMsg", msg);
			// 	sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			// }
			// if ((*channelIt)->getModes() & kRestrictTopicSetting) {
			// 	msg = ":" + info->getServerName() + " MODE " + (*channelIt)->getName() + " :+t\r\n";
			// 	debugPrintSendMessage("SendMsg", msg);
			// 	sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			// }
			// for (std::vector<User *>::const_iterator operIt = (*channelIt)->getOperators().begin(); operIt != (*channelIt)->getOperators().end(); operIt++) {
			// 	msg = ":" + info->getServerName() + " MODE " + (*channelIt)->getName() + " +o " + (*operIt)->getNickName() + "\r\n";
			// 	debugPrintSendMessage("SendMsg", msg);
			// 	sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			// }
			return ("");
		}
		// <channel>が存在しない場合
		// サーバが管理するchannel数が最大数を超えている場合
		if (info->getChannels().size() >= static_cast<unsigned long>(info->getMaxChannel())) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// <channel>を作成
		info->pushBackChannel(new Channel(parsedMsg.getParams()[0].getValue()));
		channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		// userを追加してメッセージを送信
		(*channelIt)->pushBackMember(user);
		(*channelIt)->pushBackOperator(user);
		std::string	msg = ":" + user->getNickName() + " JOIN " + (*channelIt)->getName() + "\r\n";
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		// msg = ":" + info->getServerName() + " MODE " + it->getName() + " +o " + user->getNickName() + "\r\n";
		// debugPrintSendMessage("SendMsg", msg);
		// sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		// TODO(hnoguchi): Send RPL_CHANNELMODEIS
		// JOINしたuserへchannel情報(RPL_TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES)を送信
		msg = Reply::rplFromName(info->getServerName());
		if ((*channelIt)->getTopic().size() > 0) {
			msg += Reply::rplTopic(kRPL_TOPIC, user->getNickName(), (*channelIt)->getName(), (*channelIt)->getTopic());
		} else {
			msg += Reply::rplNoTopic(kRPL_NOTOPIC, user->getNickName(), (*channelIt)->getName());
		}
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		msg = Reply::rplFromName(info->getServerName());
		msg += Reply::rplNamReply(kRPL_NAMREPLY, user->getNickName(), *(*channelIt));
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());

		msg = Reply::rplFromName(info->getServerName());
		msg += Reply::rplEndOfNames(kRPL_ENDOFNAMES, user->getNickName(), (*channelIt)->getName());
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		return ("");
	} catch (std::exception& e) {
		debugPrintErrorMessage(e.what());
		throw;
		// return ("");
	}
}
