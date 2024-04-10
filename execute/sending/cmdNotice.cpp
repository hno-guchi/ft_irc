/*
 *    Command: NOTICE
 * Parameters: <msgtarget> <text>
 *
 * Numeric Replies:
 *             411 ERR_NORECIPIENT      ":No recipient given (<command>)"
 *             412 ERR_NOTEXTTOSEND     ":No text to send"
 *             404 ERR_CANNOTSENDTOCHAN "<channel name> :Cannot send to channel"
 *             413 ERR_NOTOPLEVEL       "<mask> :No toplevel domain specified"
 *             414 ERR_WILDTOPLEVEL     "<mask> :Wildcard in toplevel domain"
 *             407 ERR_TOOMANYTARGETS   "<target> :<error code> recipients. <abort message>"
 *             401 ERR_NOSUCHNICK       "<nickname> :No such nick/channel"
 *             301 RPL_AWAY             "<nick> :<away message>"
 *
 * Examples:
 *    :Angel!wings@irc.org NOTICE Wiz :Are you receiving this message ?
 *    「:Angel!wings@irc.orgから、Wizへメッセージを送信」
 *
 *    NOTICE Angel :yes I'm receiving it !
 *    「Angelへメッセージを送る」
 *
 *    NOTICE jto@tolsun.oulu.fi :Hello !
 *    「tolsun.oulu.fiサーバのユーザ"jto"にメッセージを送る」
 *
 *    NOTICE kalt%millennium.stealth.net@irc.stealth.net :Are you a frog?
 *    「irc.stealth.netサーバのユーザ"kalt"（ホスト名millennium.stealth.net）へのメッセージ」
 *
 *    NOTICE kalt%millennium.stealth.net :Do you like cheese?
 *    「ローカルサーバーのユーザ"kalt"（ホスト名millennium.stealth.net）へのメッセージ」
 *
 *    NOTICE Wiz!jto@tolsun.oulu.fi :Hello !
 *    「ニックネーム"Wiz"（ユーザー名"jto"、ホスト名tolsun.oulu.fi）から接続しているユーザーへのメッセージ」
 *
 *    NOTICE $*.fi :Server tolsun.oulu.fi rebooting.
 *    「サーバ名が[*.fi]と一致するサーバに所属する全員へのメッセージ」
 *
 *    NOTICE #*.edu :NSFNet is undergoing work, expect interruptions
 *    「ホスト名が[*.edu]と一致するホストから来たすべてのユーザへのメッセージ」
 */

#include <iostream>
#include "../Execute.hpp"
#include "../../user/User.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"
#include "../../parser/Parser.hpp"
#include "../../error/error.hpp"

int Execute::cmdNotice(User* user, const ParsedMessage& parsedMsg, Info* info) {
	// 送り主のニックネームを取得
	std::string	message = ":" + user->getNickName() + " NOTICE ";
	// 送り先のニックネームを取得
	std::vector<Param>	params = parsedMsg.getParams();
	// メッセージを付ける
	// TODO(hnoguchi): parser classで、prefixを外すかどうか考える必要がある。
	message += params[0].getValue() + " " + params[1].getValue() + "\r\n";
	for (std::vector<User>::const_iterator it = info->getUsers().begin(); \
			it != info->getUsers().end(); it++) {
		if (params[0].getValue() == it->getNickName()) {
			int fd = it->getFd();
			// 送信
			std::cout << "Send message: [" << message << "]" << std::endl;
			ssize_t		sendMsgSize = sendNonBlocking(fd, message.c_str(), message.size());
			if (sendMsgSize <= 0) {
				// handleClientDisconnect(&it->getFd);
				return (-1);
			}
			// TODO(hnoguchi): castは使わない実装にする？？
			if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
				fatalError("send");
			}
			break;
		}
	}
	// TODO(hnoguchi): 成功用のenumを作成するかも。。。
	return (0);
}
