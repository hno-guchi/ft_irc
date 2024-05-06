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

std::string	Execute::cmdPong(User* user, const ParsedMessage& parsedMsg, Info* info) {
	try {
		// TODO(hnoguchi): Parser classでバリデーションを行う。
		if (parsedMsg.getParams().size() < 1) {
			return (Reply::errNoOrigin(kERR_NOORIGIN, user->getNickName()));
		}
		std::string	message = ":" + info->getConfig().getServerName() + " PONG " + info->getConfig().getServerName() + "\r\n";
		debugPrintSendMessage("SendMessage", message);
		sendNonBlocking(user->getFd(), message.c_str(), message.size());
		return ("");
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		// handleClientDisconnect(&user->getFd());
		// TODO(hnoguchi): usetも削除する。
		// TODO(hnoguchi): 適切なエラーナンバーを返す。
		return ("");
	}
	// TODO(hnoguchi): 成功用のenumを作成するかも。。。
}
