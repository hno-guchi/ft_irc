/*
 *    Command: PONG
 * Parameters: <server> [ <server2> ]
 *
 * Numeric Replies:
 *             409 ERR_NOORIGIN     ":No origin specified"
 *             402 ERR_NOSUCHSERVER "<server name> :No such server"
 *
 * Example:
 *    PONG csd.bu.edu tolsun.oulu.fi  ; PONG message from csd.bu.edu to tolsun.oulu.fi
 *
 */

#include <iostream>
#include "../Execute.hpp"
#include "../../user/User.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"
#include "../../parser/Parser.hpp"
#include "../../error/error.hpp"

int	Execute::cmdPong(User* user, const ParsedMessage& parsedMsg, Info* info) {
	// TODO(hnoguchi): Parser classでバリデーションを行う。
	if (parsedMsg.getParams().size() < 1) {
		return (kERR_NOORIGIN);
	}
	ssize_t		sendMsgSize = 0;
	std::string	message = ":" + info->getConfig().getServerName() + " PONG " + info->getConfig().getServerName() + "\r\n";
	debugPrintSendMessage("SendMessage", message);
	try {
		sendMsgSize = sendNonBlocking(user->getFd(), message.c_str(), message.size());
		// if (sendMsgSize <= 0) {
		// 	// handleClientDisconnect(&user->getFd());
		// 	return (-1);
		// }
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		// handleClientDisconnect(&user->getFd());
		// TODO(hnoguchi): usetも削除する。
		// TODO(hnoguchi): 適切なエラーナンバーを返す。
		return (-1);
	}
	// TODO(hnoguchi): castは使わない実装にする？？
	if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
		// handleClientDisconnect(fd);
		throw std::runtime_error("send");
	}
	// TODO(hnoguchi): 成功用のenumを作成するかも。。。
	return (0);
}
