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
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdJoin(User* user, const ParsedMessage& parsedMsg, Info* info) {
	try {
		// Check 461 ERR_NEEDMOREPARAMS  "<command> :Not enough parameters"
		if (parsedMsg.getParams().size() < 1) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		// channelを探す
		std::vector<Channel>::iterator	channelIt = const_cast<std::vector<Channel> &>(info->getChannels()).begin();
		for (; channelIt != const_cast<std::vector<Channel> &>(info->getChannels()).end(); channelIt++) {
			if (parsedMsg.getParams()[0].getValue() == channelIt->getName()) {
				break;
			}
		}
		// <channel>が存在する場合
		if (channelIt != const_cast<std::vector<Channel> &>(info->getChannels()).end()) {
			// Check 473 ERR_INVITEONLYCHAN  "<channel> :Cannot join channel (+i)"
			if (channelIt->getModes() & kInviteOnly) {
				return (Reply::errInviteOnlyChan(kERR_INVITEONLYCHAN, user->getNickName(), channelIt->getName()));
			}
			// 475 ERR_BADCHANNELKEY   "<channel> :Cannot join channel (+k)"
			if (channelIt->getModes() & kKey) {
				if (parsedMsg.getParams().size() < 2) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				} else if (parsedMsg.getParams()[1].getValue() != channelIt->getKey()) {
					return (Reply::errBadChannelKey(kERR_BADCHANNELKEY, user->getNickName(), channelIt->getName()));
				}
			}
			// 471 ERR_CHANNELISFULL   "<channel> :Cannot join channel (+l)"
			if (channelIt->getModes() & kLimit) {
				if (channelIt->getMembers().size() >= static_cast<unsigned long>(channelIt->getLimit())) {
					return (Reply::errChannelIsFull(kERR_CHANNELISFULL, user->getNickName(), channelIt->getName()));
				}
			}
			// userを<channel>に追加
			channelIt->addMember(user);
			std::string	msg = ":" + user->getNickName() + " JOIN " + channelIt->getName() + "\r\n";
			debugPrintSendMessage("SendMsg", msg);
			for (std::vector<User *>::const_iterator memberIt = channelIt->getMembers().begin(); memberIt != channelIt->getMembers().end(); memberIt++) {
				sendNonBlocking((*memberIt)->getFd(), msg.c_str(), msg.size());
				// TODO(hnoguchi): Check error
			}
			// TODO(hnoguchi): <channel>情報を送る。RPL_CHANNELMODEIS
			// sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			if (channelIt->getTopic().size() > 0) {
				return (Reply::rplTopic(kRPL_TOPIC, user->getNickName(), channelIt->getName(), channelIt->getTopic()));
			}
			return (Reply::rplNoTopic(kRPL_NOTOPIC, user->getNickName(), channelIt->getName()));
		}
		// <channel>が存在しない場合
		// サーバが管理するchannel数が最大数を超えている場合
		if (info->getChannels().size() >= static_cast<unsigned long>(info->getConfig().getMaxChannel())) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// <channel>を作成
		info->addChannel(Channel(parsedMsg.getParams()[0].getValue()));
		for (std::vector<Channel>::iterator it = const_cast<std::vector<Channel> &>(info->getChannels()).begin(); it != const_cast<std::vector<Channel> &>(info->getChannels()).end(); it++) {
			std::cerr << "In for()" << std::endl;
			// userを追加してメッセージを送信
			if (parsedMsg.getParams()[0].getValue() == it->getName()) {
				it->addMember(user);
				it->addOperator(user);
				std::string	msg = ":" + user->getNickName() + " JOIN " + it->getName() + "\r\n";
				msg += ":" + info->getConfig().getServerName() + " MODE " + it->getName() + " +o " + user->getNickName() + "\r\n";
				// TODO(hnoguchi): Send RPL_CHANNELMODEIS
				debugPrintSendMessage("SendMsg", msg);
				sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
				// TODO(hnoguchi): Check error
				return (Reply::rplNoTopic(kRPL_NOTOPIC, user->getNickName(), channelIt->getName()));
			}
		}

	} catch (std::exception& e) {
		std::cerr << RED << e.what() << END << std::endl;
	}
	return ("");
}
