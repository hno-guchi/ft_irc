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
#include "../../error/error.hpp"

int	Execute::cmdNotice(User* user, const ParsedMessage& parsedMsg, Info* info) {
	// TODO(hnoguchi): Parser classで処理する。
	if (parsedMsg.getParams().size() == 0) {
		return(kERR_NORECIPIENT);
	}
	if (parsedMsg.getParams().size() == 1) {
		if (parsedMsg.getParams()[0].getType() == kPTrailing) {
			return(kERR_NORECIPIENT);
		} else if (parsedMsg.getParams()[0].getType() == kPMiddle) {
			return(kERR_NOTEXTTOSEND);
		}
	}
	try {
		// 送り主のニックネームを取得
		std::string	message = ":" + user->getNickName() + " NOTICE ";
		// 送り先のニックネームを取得
		// std::vector<Param>	params = parsedMsg.getParams();
		// メッセージを付ける
		message += parsedMsg.getParams()[0].getValue() + " " + parsedMsg.getParams()[1].getValue() + "\r\n";
		std::vector<User>::const_iterator	it = info->getUsers().begin();
		for (; it != info->getUsers().end(); it++) {
			if (parsedMsg.getParams()[0].getValue() == it->getNickName()) {
				break;
			}
		}
		if (it == info->getUsers().end()) {
			return (kERR_NOSUCHNICK);
		}
		// 送信
		debugPrintSendMessage("cmdNotice", message);
		ssize_t		sendMsgSize = sendNonBlocking(it->getFd(), message.c_str(), message.size());
		if (sendMsgSize <= 0) {
			// handleClientDisconnect(&it->getFd);
			return (-1);
		}
		// TODO(hnoguchi): castは使わない実装にする？？
		if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
			fatalError("send");
		}
		// TODO(hnoguchi): 成功用のenumを作成するかも。。。
		return (0);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		// handleClientDisconnect(&it->getFd);
		return (-1);
	}
}
