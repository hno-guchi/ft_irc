/*
 *    Command: MODE
 * Parameters: <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
 *             467 ERR_KEYSET           "<channel> :Channel key already set"
 *             477 ERR_NOCHANMODES      "<channel> :Channel doesn't support modes"
 *             482 ERR_CHANOPRIVSNEEDED "<channel> :You're not channel operator"
 *             441 ERR_USERNOTINCHANNEL "<nick> <channel> :They aren't on that channel"
 *             472 ERR_UNKNOWNMODE      "<char> :is unknown mode char to me for <channel>"
 *             324 RPL_CHANNELMODEIS    "<channel> <mode> <mode params>"
 *             367 RPL_BANLIST          "<channel> <banmask>"
 *             368 RPL_ENDOFBANLIST     "<channel> :End of channel ban list"
 *             348 RPL_EXCEPTLIST       "<channel> <exceptionmask>"
 *             349 RPL_ENDOFEXCEPTLIST  "<channel> :End of channel exception list"
 *             346 RPL_INVITELIST       "<channel> <invitemask>"
 *             347 RPL_ENDOFINVITELIST  "<channel> :End of channel invite list"
 *             325 RPL_UNIQOPIS         "<channel> <nickname>"
 *
 * Examples:
 *    MODE #Finnish +imI *!*@*.fi     ; Command to make #Finnish channel moderated and 'invite-only' with user with a hostname matching *.fi automatically invited.
 *    MODE #Finnish +o Kilroy         ; Command to give 'chanop' privileges to Kilroy on channel #Finnish.
 *    MODE #Finnish +v Wiz            ; Command to allow WiZ to speak on #Finnish.
 *    MODE #Fins -s                   ; Command to remove 'secret' flag from channel #Fins.
 *    MODE #42 +k oulu                ; Command to set the channel key to "oulu".
 *    MODE #42 -k oulu                ; Command to remove the "oulu" channel key on channel "#42".
 *    MODE #eu-opers +l 10            ; Command to set the limit for the number of users on channel "#eu-opers" to 10.
 *    :WiZ!jto@tolsun.oulu.fi MODE #eu-opers -l
 *                                    ; User "WiZ" removing the limit for the number of users on channel "#eu-opers".
 *    MODE &oulu +b                   ; Command to list ban masks set for the channel "&oulu".
 *    MODE &oulu +b *!*@*             ; Command to prevent all users from joining.
 *    MODE &oulu +b *!*@*.edu +e *!*@*.bu.edu
 *                                    ; Command to prevent any user from a hostname matching *.edu from joining, except if matching *.bu.edu
 *    MODE #bu +be *!*@*.edu *!*@*.bu.edu
 *                                    ; Comment to prevent any user from a hostname matching *.edu from joining, except if matching *.bu.edu
 *    MODE #meditation e              ; Command to list exception masks set for the channel "#meditation".
 *    MODE #meditation I              ; Command to list invitations masks set for the channel "#meditation".
 *    MODE !12345ircd O               ; Command to ask who the channel creator for "!12345ircd" is
 *
 *    モードを変更。メッセージを作成し送信する。"iklot"
 *    i - 招待専用チャンネルフラグ（the invite-only channel flag）の切り替え
 *    k - チャンネル・キー（the channel key）(パスワード)の設定/解除
 *    l - channelへのユーザ制限の設定/解除
 *    o - 「Channel operator」の権限を付与/剥奪
 *    t - チャネル運営者のみが設定可能なトピックフラグ設定の切り替え
 */

#include <vector>
#include <cstdlib>
#include "../Execute.hpp"
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

// TODO(hnoguchi): MODE(channel)コマンドによる変更は、チャンネルに所属するユーザに通知する必要があるものもある。
std::string	Execute::cmdChannelMode(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		if (parsedMsg.getParams().size() < 1) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		std::vector<Channel*>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channelIt == info->getChannels().end()) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		// RPL_CHANNELMODEIS を返す。
		if (parsedMsg.getParams().size() == 1) {
			// TODO(hnoguchi): Info::getChannelByName(std::string name);を実装する。
			if ((*channelIt)->getModes() & kInviteOnly) {
				std::string	reply = Reply::rplFromName(user->getNickName());
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getNickName(), parsedMsg.getParams()[0].getValue(), 'i', "");
				debugPrintSendMessage("SendMsg", reply);
				sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			if ((*channelIt)->getModes() & kKeySet) {
				std::string	reply = Reply::rplFromName(user->getNickName());
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getNickName(), parsedMsg.getParams()[0].getValue(), 'k', (*channelIt)->getKey());
				debugPrintSendMessage("SendMsg", reply);
				sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			if ((*channelIt)->getModes() & kLimit) {
				std::string			reply = Reply::rplFromName(user->getNickName());
				std::stringstream	ss;
				ss << (*channelIt)->getLimit();
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getNickName(), parsedMsg.getParams()[0].getValue(), 'l', ss.str());
				debugPrintSendMessage("SendMsg", reply);
				sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			if ((*channelIt)->getModes() & kRestrictTopicSetting) {
				std::string	reply = Reply::rplFromName(user->getNickName());
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getNickName(), parsedMsg.getParams()[0].getValue(), 't', "");
				debugPrintSendMessage("SendMsg", reply);
				sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			std::string	reply = Reply::rplFromName(user->getNickName());
			reply += Reply::errNoChanModes(kERR_NOCHANMODES, user->getNickName(), user->getNickName(), parsedMsg.getParams()[0].getValue());
			return (reply);
		}
		// channel operatorか確認する。
		if (!(*channelIt)->isOperator(user->getNickName())) {
			return (Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getNickName(), user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		if (parsedMsg.getParams()[1].getValue().size() != 2) {
			return (Reply::errUnknownMode(kERR_UNKNOWNMODE, user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
		}
		if (parsedMsg.getParams()[1].getValue()[0] != '+' && parsedMsg.getParams()[1].getValue()[0] != '-') {
			return (Reply::errUnknownMode(kERR_UNKNOWNMODE,user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
		}
		unsigned long	pos = info->getChannelModes().find(parsedMsg.getParams()[1].getValue()[1]);
		if (pos == std::string::npos) {
			return (Reply::errUnknownMode(kERR_UNKNOWNMODE,user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
		}
		// そのチャンネルで使用できるモードか確認する。
		std::string	msg = ":" + user->getNickName() + " MODE " + (*channelIt)->getName() + " " + parsedMsg.getParams()[1].getValue();
		if (parsedMsg.getParams()[1].getValue()[0] == '+') {
			if (parsedMsg.getParams()[1].getValue()[1] == 'i') {
				// TODO(hnoguchi): Change prefix
				(*channelIt)->setMode(kInviteOnly);
				msg += "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'k') {
				// keyフラグが既に立っているか確認する。
				if ((*channelIt)->getModes() & kKeySet) {
					return (Reply::errKeySet(kERR_KEYSET, user->getNickName(), user->getNickName(), (*channelIt)->getName()));
				}
				if (parsedMsg.getParams().size() < 3) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				}
				// TODO(hnoguchi): Validate string key
				(*channelIt)->setKey(parsedMsg.getParams()[2].getValue());
				(*channelIt)->setMode(kKeySet);
				msg += " " + parsedMsg.getParams()[2].getValue() + "\r\n";
				std::cerr << "key: " << (*channelIt)->getKey() << std::endl;
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'l') {
				if (parsedMsg.getParams().size() < 3) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				}
				// TODO(hnoguchi): Validate Value
				(*channelIt)->setLimit(std::atoi(parsedMsg.getParams()[2].getValue().c_str()));
				(*channelIt)->setMode(kLimit);
				msg += " " + parsedMsg.getParams()[2].getValue() + "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'o') {
				if (parsedMsg.getParams().size() < 3) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				}
				if (!(*channelIt)->isMember(parsedMsg.getParams()[2].getValue())) {
					return (Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getNickName(), parsedMsg.getParams()[2].getValue(), (*channelIt)->getName()));
				}
				(*channelIt)->pushBackOperator(*info->findUser(parsedMsg.getParams()[2].getValue()));
				msg += " " + parsedMsg.getParams()[2].getValue() + "\r\n";
				// debugPrintSendMessage("SendMsg", msg);
				// sendNonBlocking(*((*targetUserIt)->getFd()), msg.c_str(), msg.size());
			} else if (parsedMsg.getParams()[1].getValue()[1] == 't') {
				(*channelIt)->setMode(kRestrictTopicSetting);
				msg += "\r\n";
			}
		} else if (parsedMsg.getParams()[1].getValue()[0] == '-') {
			if (parsedMsg.getParams()[1].getValue()[1] == 'i') {
				// TODO(hnoguchi): Change prefix
				(*channelIt)->unsetMode(kInviteOnly);
				msg += "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'k') {
				(*channelIt)->setKey("");
				(*channelIt)->unsetMode(kKeySet);
				msg += "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'l') {
				(*channelIt)->setLimit(0);
				(*channelIt)->unsetMode(kLimit);
				msg += "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'o') {
				if (parsedMsg.getParams().size() < 3) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				}
				if ((*channelIt)->isMember(parsedMsg.getParams()[2].getValue())) {
					return (Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getNickName(), parsedMsg.getParams()[2].getValue(), (*channelIt)->getName()));
				}
				if ((*channelIt)->isOperator(parsedMsg.getParams()[2].getValue())) {
					return (Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getNickName(), parsedMsg.getParams()[2].getValue(), (*channelIt)->getName()));
				}
				(*channelIt)->eraseOperator(*info->findUser(parsedMsg.getParams()[2].getValue()));
				msg += parsedMsg.getParams()[2].getValue() + "\r\n";
				debugPrintSendMessage("SendMsg", msg);
				sendNonBlocking((*info->findUser(parsedMsg.getParams()[2].getValue()))->getFd(), msg.c_str(), msg.size());
			} else if (parsedMsg.getParams()[1].getValue()[1] == 't') {
				(*channelIt)->unsetMode(kRestrictTopicSetting);
				msg += "\r\n";
			}
		}
		debugPrintSendMessage("SendMsg", msg);
		for (std::vector<User*>::const_iterator it = (*channelIt)->getMembers().begin(); it != (*channelIt)->getMembers().end(); it++) {
			sendNonBlocking((*it)->getFd(), msg.c_str(), msg.size());
		}
		return ("");
	} catch (const std::exception& e) {
		debugPrintErrorMessage(e.what());
		throw;
		// return ("");
	}
}
