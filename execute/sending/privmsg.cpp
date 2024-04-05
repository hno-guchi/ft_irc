/*
 *    Command: PRIVMSG
 * Parameters: <msgtarget> <text to be sent>
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
 *    :Angel!wings@irc.org PRIVMSG Wiz :Are you receiving this message ?
 *    「:Angel!wings@irc.orgから、Wizへメッセージを送信」
 *
 *    PRIVMSG Angel :yes I'm receiving it !
 *    「Angelへメッセージを送る」
 *
 *    PRIVMSG jto@tolsun.oulu.fi :Hello !
 *    「tolsun.oulu.fiサーバのユーザ"jto"にメッセージを送る」
 *
 *    PRIVMSG kalt%millennium.stealth.net@irc.stealth.net :Are you a frog?
 *    「irc.stealth.netサーバのユーザ"kalt"（ホスト名millennium.stealth.net）へのメッセージ」
 *
 *    PRIVMSG kalt%millennium.stealth.net :Do you like cheese?
 *    「ローカルサーバーのユーザ"kalt"（ホスト名millennium.stealth.net）へのメッセージ」
 *
 *    PRIVMSG Wiz!jto@tolsun.oulu.fi :Hello !
 *    「ニックネーム"Wiz"（ユーザー名"jto"、ホスト名tolsun.oulu.fi）から接続しているユーザーへのメッセージ」
 *
 *    PRIVMSG $*.fi :Server tolsun.oulu.fi rebooting.
 *    「サーバ名が[*.fi]と一致するサーバに所属する全員へのメッセージ」
 *
 *    PRIVMSG #*.edu :NSFNet is undergoing work, expect interruptions
 *    「ホスト名が[*.edu]と一致するホストから来たすべてのユーザへのメッセージ」
 */

#include <iostream>
#include <vector>
#include "../Execute.hpp"
#include "../../user/User.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"
#include "../../parser/Parser.hpp"
#include "../../error/error.hpp"

int Execute::privmsg(User* user, const ParsedMessage& parsedMsg, Info* info) {
	// channelがなければ、エラーリプライナンバーを返す
	// channelを探す
	for (std::vector<Channel>::const_iterator it = info->getChannels().begin(); \
			it != info->getChannels().end(); it++) {
		// channelがあれば、メンバーにメッセージを送信する
		if (parsedMsg.getParams()[0].getValue() == it->getName()) {
			// TODO(hnoguchi): このやり方は、良くないと思うので、改善が必要。
			const std::vector<User*>& members = it->getMembers();
			for (std::vector<User*>::const_iterator member = members.begin(); \
					member != members.end(); member++) {
				if (user->getNickName() == (*member)->getNickName()) {
					continue;
				}
				// 送り主のニックネームを取得
				std::string	message = ":" + user->getNickName() + " PRIVMSG ";
				// 送り先のchannelを取得
				message += parsedMsg.getParams()[0].getValue() + " " + parsedMsg.getParams()[1].getValue() + "\r\n";
				std::cout << "Send message: [" << message << "]" << std::endl;
				ssize_t		sendMsgSize = sendNonBlocking((*member)->getFd(), message.c_str(), message.size());
				if (sendMsgSize <= 0) {
					// handleClientDisconnect(&(*member)->getFd());
					return (-1);
				}
				// TODO(hnoguchi): castは使わない実装にする？？
				if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
					fatalError("send PRIVMSG");
				}
			}
		}
	}
	// TODO(hnoguchi): 成功用のenumを作成するかも。。。
	return (0);
}
