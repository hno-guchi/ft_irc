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

void	Execute::cmdJoin(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());
		// channelを探す
		Channel*	channel = info->findChannel(parsedMsg.getParams()[0].getValue());
		// <channel>が存在する場合
		if (channel != NULL) {
			if (channel->getModes() & kInviteOnly) {
				if (!channel->isInvited(user)) {
					reply += Reply::errInviteOnlyChan(kERR_INVITEONLYCHAN, user->getPrefixName(), channel->getName());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				channel->eraseInvited(user);
			}
			if (channel->getModes() & kKeySet) {
				if (parsedMsg.getParams().size() < 2) {
					reply += Reply::errBadChannelKey(kERR_BADCHANNELKEY, user->getPrefixName(), channel->getName());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				if (parsedMsg.getParams()[1].getValue() != channel->getKey()) {
					reply += Reply::errBadChannelKey(kERR_BADCHANNELKEY, user->getPrefixName(), channel->getName());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
			}
			if (channel->getModes() & kLimit) {
				if (channel->getMembers().size() >= static_cast<unsigned long>(channel->getLimit())) {
					reply += Reply::errChannelIsFull(kERR_CHANNELISFULL, user->getPrefixName(), channel->getName());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
			}
			// userを<channel>に追加
			channel->pushBackMember(user);
			std::string	msg = ":" + user->getPrefixName() + " JOIN " + channel->getName() + Reply::getDelimiter();
#ifdef DEBUG
			channel->debugPrintChannel();
#endif  // DEBUG
			for (std::vector<User*>::const_iterator memberIt = channel->getMembers().begin(); memberIt != channel->getMembers().end(); memberIt++) {
				Server::sendNonBlocking((*memberIt)->getFd(), msg.c_str(), msg.size());
			}
			// JOINしたuserへchannel情報(RPL_(NO)TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES)を送信
			msg = Reply::rplFromName(info->getServerName());
			if (channel->getTopic().size() > 0) {
				msg += Reply::rplTopic(kRPL_TOPIC, user->getPrefixName(), channel->getName(), channel->getTopic());
			} else {
				msg += Reply::rplNoTopic(kRPL_NOTOPIC, user->getPrefixName(), channel->getName());
			}
			Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());

			msg = Reply::rplFromName(info->getServerName());
			msg += Reply::rplNamReply(kRPL_NAMREPLY, user->getPrefixName(), *channel);
			msg += Reply::rplFromName(info->getServerName());
			msg += Reply::rplEndOfNames(kRPL_ENDOFNAMES, user->getPrefixName(), channel->getName());
			Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
			return;
		}
		// <channel>が存在しない場合
		// サーバが管理するchannel数が最大数を超えている場合
		if (info->getChannels().size() >= static_cast<unsigned long>(info->getMaxChannel())) {
			reply += Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// <channel>を作成
		channel = new Channel(parsedMsg.getParams()[0].getValue());
		info->pushBackChannel(channel);
		// userを追加してメッセージを送信
		channel->pushBackMember(user);
		channel->pushBackOperator(user);
		std::string	msg = ":" + user->getPrefixName() + " JOIN " + channel->getName() + Reply::getDelimiter();
		Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		msg = Reply::rplFromName(info->getServerName());
		if (channel->getTopic().size() > 0) {
			msg += Reply::rplTopic(kRPL_TOPIC, user->getPrefixName(), channel->getName(), channel->getTopic());
		} else {
			msg += Reply::rplNoTopic(kRPL_NOTOPIC, user->getPrefixName(), channel->getName());
		}
		Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		msg = Reply::rplFromName(info->getServerName());
		msg += Reply::rplNamReply(kRPL_NAMREPLY, user->getPrefixName(), *channel);
		Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());

		msg = Reply::rplFromName(info->getServerName());
		msg += Reply::rplEndOfNames(kRPL_ENDOFNAMES, user->getPrefixName(), channel->getName());
		Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		// TODO(hnoguchi): Send RPL_CHANNELMODEIS
		// msg = Reply::rplFromName(info->getServerName());
		// msg += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getPrefixName(), *channel);
		// Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
