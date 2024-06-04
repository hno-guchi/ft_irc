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

std::string	Execute::cmdNotice(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		// TODO(hnoguchi): Parser classで処理する。
		if (parsedMsg.getParams().size() == 0) {
			return(Reply::errNoRecipient(kERR_NORECIPIENT, user->getNickName(), parsedMsg.getCommand()));
		}
		if (parsedMsg.getParams().size() < 2) {
			if (parsedMsg.getParams()[0].getParamType() == kText) {
				return(Reply::errNoRecipient(kERR_NORECIPIENT, user->getNickName(), parsedMsg.getCommand()));
			} else if (parsedMsg.getParams()[0].getParamType() == kMsgTarget) {
				return(Reply::errNoTextToSend(kERR_NOTEXTTOSEND, user->getNickName()));
			}
		}
		std::vector<User*>::const_iterator	userIt = info->findUser(parsedMsg.getParams()[0].getValue());
		// メッセージの作成
		std::string	message = ":" + user->getNickName() + " NOTICE " + parsedMsg.getParams()[0].getValue() + " :" + parsedMsg.getParams()[1].getValue() + "\r\n";
		debugPrintSendMessage("cmdNotice", message);
		// メッセージの送信先がuserの場合
		if (userIt != info->getUsers().end()) {
			sendNonBlocking((*userIt)->getFd(), message.c_str(), message.size());
			return ("");
		}
		// メッセージの送信先がchannelの場合
		// <channel>が存在するか確認
		std::vector<Channel*>::const_iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channelIt == info->getChannels().end()) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// userが<channel>にいるか確認
		if (!(*channelIt)->isMember(user->getNickName())) {
			return (Reply::errCanNotSendToChan(kERR_CANNOTSENDTOCHAN, user->getNickName(), (*channelIt)->getName()));
		}
		for (std::vector<User*>::const_iterator memberIt = (*channelIt)->getMembers().begin(); memberIt != (*channelIt)->getMembers().end(); memberIt++) {
			if (user->getNickName() == (*memberIt)->getNickName()) {
				continue;
			}
			sendNonBlocking((*memberIt)->getFd(), message.c_str(), message.size());
		}
		return ("");
	} catch (std::exception& e) {
		debugPrintErrorMessage(e.what());
		// handleClientDisconnect(&it->getFd);
		throw;
		// return ("");
	}
}
