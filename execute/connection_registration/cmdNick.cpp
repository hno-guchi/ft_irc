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
#include "../../error/error.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

std::string	Execute::cmdNick(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		// TODO(hnoguchi): Parser classでバリデーションを行う？
		if (parsedMsg.getParams().size() == 0) {
			return (Reply::errNoNickNameGiven(kERR_NONICKNAMEGIVEN, user->getNickName()));
		}
		// if (parsedMsg.getParams().size() > 1) {
		// 	return ("");
		// }
		// TODO(hnoguchi): Parser classでバリデーションを行う。
		if (parsedMsg.getParams()[0].getValue().size() > 9) {
			return (Reply::errOneUsNickName(kERR_ERRONEUSNICKNAME, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
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
				return (Reply::errNickNameInUse(kERR_NICKNAMEINUSE, user->getReplyName(), nick));
			}
			user->setNickName(nick);
			std::string	msg = ":" + user->getReplyName() + " NICK :" + nick + "\r\n";
			debugPrintSendMessage("SendMsg", msg);
			sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		}
		return ("");
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return ("");
	}
}
