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
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"
#include "../../parser/Parser.hpp"
#include "../../error/error.hpp"

std::string	Execute::cmdPrivmsg(User* user, const ParsedMessage& parsedMsg, Info* info) {
	try {
		// TODO(hnoguchi): Parser classで処理する。
		if (parsedMsg.getParams().size() == 0) {
			return(Reply::errNoRecipient(kERR_NORECIPIENT, user->getNickName(), parsedMsg.getCommand()));
		}
		if (parsedMsg.getParams().size() < 2) {
			if (parsedMsg.getParams()[0].getType() == kPTrailing) {
				return(Reply::errNoRecipient(kERR_NORECIPIENT, user->getNickName(), parsedMsg.getCommand()));
			} else if (parsedMsg.getParams()[0].getType() == kPMiddle) {
				return(Reply::errNoTextToSend(kERR_NOTEXTTOSEND, user->getNickName()));
			}
		}
		std::vector<User>::const_iterator	it = info->getUsers().begin();
		for (; it != info->getUsers().end(); it++) {
			if (parsedMsg.getParams()[0].getValue() == it->getNickName()) {
				break;
			}
		}
		// メッセージの作成
		std::string	message = ":" + user->getNickName() + " PRIVMSG " + parsedMsg.getParams()[0].getValue() + " " + parsedMsg.getParams()[1].getValue() + "\r\n";
		debugPrintSendMessage("cmdPrivMsg", message);
		// メッセージの送信先がuserの場合
		if (it != info->getUsers().end()) {
			// 送信
			ssize_t		sendMsgSize = sendNonBlocking(it->getFd(), message.c_str(), message.size());
			if (sendMsgSize <= 0) {
				// handleClientDisconnect(&it->getFd);
				return ("");
			}
			// TODO(hnoguchi): castは使わない実装にする？？
			if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
				fatalError("send");
			}
			return ("");
		}
		// メッセージの送信先がchannelの場合
		std::vector<Channel>::iterator	channelIt = const_cast<std::vector<Channel> &>(info->getChannels()).begin();
		for (; channelIt != info->getChannels().end(); channelIt++) {
			if (parsedMsg.getParams()[0].getValue() == channelIt->getName()) {
				break;
			}
		}
		if (channelIt == info->getChannels().end()) {
			// std::cerr << "No such channel" << std::endl;
			return (Reply::errNoSuchNick(kERR_NOSUCHNICK, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		std::vector<User*>::const_iterator	memberIt = channelIt->getMembers().begin();
		for (; memberIt != channelIt->getMembers().end(); memberIt++) {
			if (user->getNickName() == (*memberIt)->getNickName()) {
				break;
			}
		}
		if (memberIt == channelIt->getMembers().end()) {
			return (Reply::errCanNotSendToChan(kERR_CANNOTSENDTOCHAN, user->getNickName(), channelIt->getName()));
		}
		memberIt = channelIt->getMembers().begin();
		for (; memberIt != channelIt->getMembers().end(); memberIt++) {
			if (user->getNickName() == (*memberIt)->getNickName()) {
				continue;
			}
			ssize_t		sendMsgSize = sendNonBlocking((*memberIt)->getFd(), message.c_str(), message.size());
			if (sendMsgSize <= 0) {
				// handleClientDisconnect(&(*member)->getFd());
				return ("");
			}
			// TODO(hnoguchi): castは使わない実装にする？？
			if (static_cast<ssize_t>(message.size()) != sendMsgSize) {
				fatalError("send PRIVMSG");
			}
		}
		return ("");
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		// handleClientDisconnect(&it->getFd);
		return ("");
	}
}
