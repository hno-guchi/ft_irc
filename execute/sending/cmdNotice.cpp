/*
 *    Command: NOTICE
 * Parameters: <msgtarget> <text>
 *
 * Numeric Replies:
 *             411 ERR_NORECIPIENT      ":No recipient given (<command>)"
 *             412 ERR_NOTEXTTOSEND     ":No text to send"
 *             404 ERR_CANNOTSENDTOCHAN "<channel name> :Cannot send to channel"
 *             [ 413 ERR_NOTOPLEVEL       "<mask> :No toplevel domain specified"
 *             [ 414 ERR_WILDTOPLEVEL     "<mask> :Wildcard in toplevel domain"
 *             [ 407 ERR_TOOMANYTARGETS   "<target> :<error code> recipients. <abort message>"
 *             401 ERR_NOSUCHNICK       "<nickname> :No such nick/channel"
 *             [ 301 RPL_AWAY             "<nick> :<away message>"
 */

#include <iostream>
#include "../Execute.hpp"
#include "../../user/User.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"
#include "../../parser/Parser.hpp"
#include "../../parser/ParsedMsg.hpp"
#include "../../debug/debug.hpp"

void	Execute::cmdNotice(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		User*	targetUser = info->findUser(parsedMsg.getParams()[0].getValue());
		// メッセージの作成
		std::string	message = ":" + user->getPrefixName() + " NOTICE " + parsedMsg.getParams()[0].getValue() + " " + parsedMsg.getParams()[1].getValue() + Reply::getDelimiter();
		// メッセージの送信先がuserの場合
		if (targetUser != NULL) {
			Server::sendNonBlocking(targetUser->getFd(), message.c_str(), message.size());
			return;
		}
		// メッセージの送信先がchannelの場合
		// <channel>が存在するか確認
		std::string	reply = Reply::rplFromName(info->getServerName());
		Channel*	channel = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channel == NULL) {
			reply += Reply::errNoSuchNick(kERR_NOSUCHNICK, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// userが<channel>にいるか確認
		if (!channel->isMember(user)) {
			reply += Reply::errCanNotSendToChan(kERR_CANNOTSENDTOCHAN, user->getPrefixName(), channel->getName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		for (std::vector<User*>::const_iterator memberIt = channel->getMembers().begin(); memberIt != channel->getMembers().end(); memberIt++) {
			if (user->getNickName() == (*memberIt)->getNickName()) {
				continue;
			}
			Server::sendNonBlocking((*memberIt)->getFd(), message.c_str(), message.size());
		}
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
