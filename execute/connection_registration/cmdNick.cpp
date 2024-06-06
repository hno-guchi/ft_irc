/*
 * Command   : NICK
 * Parameters: <nickname>
 *
 * Numeric Replies:
 *             431 ERR_NONICKNAMEGIVEN  ":No nickname given"
 *             432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
 *             433 ERR_NICKNAMEINUSE    "<nick> :Nickname is already in use"
 *             [436 ERR_NICKCOLLISION    "<nick> :Nickname collision KILL from <user>@<host>"]
 *             [437 ERR_UNAVAILRESOURCE "<nick/channel> :Nick/channel is temporarily unavailable"]
 *             484 ERR_RESTRICTED      ":Your connection is restricted!"
 * Examples:
 *    NICK Wiz        ; Introducing new nick "Wiz" if session is still unregistered, or user changing his nickname to "Wiz"
 *    :WiZ!jto@tolsun.oulu.fi NICK Kilroy
 *                    ; Server telling that WiZ changed his nickname to Kilroy.
 */

#include <vector>
#include "../Execute.hpp"
#include "../../user/User.hpp"
#include "../../debug/debug.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

void	Execute::cmdNick(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::string	reply = Reply::rplFromName(info->getServerName());
		std::string	nick = parsedMsg.getParams()[0].getValue();

		if (!(user->getRegistered() & kNickCommand)) {
			// ユーザ登録処理
			char	sufix = '0';
			while (info->findUser(nick) != info->getUsers().end()) {
				sufix += 1;
				nick = parsedMsg.getParams()[0].getValue() + sufix;
			}
			user->setNickName(nick);
		} else {
			// 既存ユーザのニックネーム変更処理
			if (info->findUser(nick) != info->getUsers().end()) {
				reply += Reply::errNickNameInUse(kERR_NICKNAMEINUSE, user->getPrefixName(), nick);
				Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
				return ;
			}
			std::string	msg = ":" + user->getPrefixName() + " NICK :" + nick + "\r\n";
			user->setNickName(nick);
			Server::sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		}
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
