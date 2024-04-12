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
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

int	Execute::cmdNick(User* user, const ParsedMessage& parsedMsg, Info* info) {
	// TODO(hnoguchi): Parser classでバリデーションを行う。
	if (parsedMsg.getParams().size() == 0) {
		return (kERR_NONICKNAMEGIVEN);
	}
	// TODO(hnoguchi): Parser classでバリデーションを行う。
	if (parsedMsg.getParams().size() > 1) {
		return (kERR_ERRONEUSNICKNAME);
	}
	// TODO(hnoguchi): Parser classでバリデーションを行う。
	if (parsedMsg.getParams()[0].getValue().size() > 9) {
		return (kERR_ERRONEUSNICKNAME);
	}
	// TODO(hnoguchi): ユーザ登録時の処理で、既に使用されている場合、sufixに数字を付与し、再度重複チェックを行う。
	try {
		std::string	nick = parsedMsg.getParams()[0].getValue();
		if (!(user->getRegistered() & kNickCommand)) {
			// std::string							nick = parsedMsg.getParams()[0].getValue();
			char								sufix = '1';
			std::vector<User>::const_iterator	it = info->getUsers().begin();
			while (it != info->getUsers().end()) {
				if (it->getNickName() == nick) {
					sufix += 1;
					nick = nick + sufix;
					it = info->getUsers().begin();
				} else {
					it += 1;
				}
			}
			// user->setNickName(nick);
		} else {
			for (std::vector<User>::const_iterator it = info->getUsers().begin(); it != info->getUsers().end(); it++) {
				// if (it->getNickName() == parsedMsg.getParams()[0].getValue()) {
				if (it->getNickName() == nick) {
					return (kERR_NICKNAMEINUSE);
				}
			}
			// user->setNickName(parsedMsg.getParams()[0].getValue());
		}
		user->setNickName(nick);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (kERR_ERRONEUSNICKNAME);
	}
	// TODO(hnoguchi): Server間通信機能を追加するときは、channelに所属するユーザのnickもチェックする。
	return (0);
}
