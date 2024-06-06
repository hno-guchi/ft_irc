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
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"
#include "../../parser/Parser.hpp"
#include "../../parser/ParsedMsg.hpp"

void	Execute::cmdPrivmsg(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::vector<User*>::const_iterator	userIt = info->findUser(parsedMsg.getParams()[0].getValue());
		// メッセージの作成
		std::string	message = ":" + user->getPrefixName() + " PRIVMSG " + parsedMsg.getParams()[0].getValue() + " " + parsedMsg.getParams()[1].getValue() + Reply::getDelimiter();
		// メッセージの送信先がuserの場合
		if (userIt != info->getUsers().end()) {
			Server::sendNonBlocking((*userIt)->getFd(), message.c_str(), message.size());
			return;
		}
		// メッセージの送信先がchannelの場合
		// <channel>が存在するか確認
		std::string	reply = Reply::rplFromName(info->getServerName());
		std::vector<Channel*>::const_iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channelIt == info->getChannels().end()) {
			reply += Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// userが<channel>にいるか確認
		if (!(*channelIt)->isMember(user->getNickName())) {
			reply += Reply::errCanNotSendToChan(kERR_CANNOTSENDTOCHAN, user->getPrefixName(), (*channelIt)->getName());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		for (std::vector<User*>::const_iterator memberIt = (*channelIt)->getMembers().begin(); memberIt != (*channelIt)->getMembers().end(); memberIt++) {
			if (user->getNickName() == (*memberIt)->getNickName()) {
				continue;
			}
			Server::sendNonBlocking((*memberIt)->getFd(), message.c_str(), message.size());
		}
		return;
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
