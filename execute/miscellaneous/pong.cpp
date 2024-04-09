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

int	Execute::pong(User* user, const ParsedMessage& parsedMsg, Info* info) {
	(void)parsedMsg;
	std::string	message = ":" + user->getNickName() + " PONG " + info->getConfig().getServerName() + "\r\n";
	std::cout << "Send message: [" << message << "]" << std::endl;
	ssize_t		sendMsgSize = sendNonBlocking(user->getFd(), message.c_str(), message.size());
	if (sendMsgSize <= 0) {
		// handleClientDisconnect(&user->getFd());
		return (-1);
	}
	// TODO(hnoguchi): castは使わない実装にする？？
	if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
		fatalError("send");
	}
	// TODO(hnoguchi): 成功用のenumを作成するかも。。。
	return (0);
}
