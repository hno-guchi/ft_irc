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
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

// TODO(hnoguchi): MODE(channel)コマンドによる変更は、チャンネルに所属するユーザに通知する必要があるものもある。
std::string	Execute::cmdChannelMode(User* user, const ParsedMessage& parsedMsg, Info* info) {
	try {
		if (parsedMsg.getParams().size() < 1) {
			return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
		}
		std::vector<Channel>::iterator	channelIt = const_cast<std::vector<Channel> &>(info->getChannels()).begin();
		for (; channelIt != info->getChannels().end(); channelIt++) {
			if (parsedMsg.getParams()[0].getValue() == channelIt->getName()) {
				break;
			}
		}
		if (channelIt == info->getChannels().end()) {
			return (Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		std::vector<User*>::iterator	operIt = const_cast<std::vector<User*> &>(channelIt->getOperators()).begin();
		for (; operIt != channelIt->getOperators().end(); operIt++) {
			if ((*operIt)->getNickName() == user->getNickName()) {
				break;
			}
		}
		if (operIt == channelIt->getOperators().end()) {
			return (Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getNickName(), user->getNickName(), parsedMsg.getParams()[0].getValue()));
		}
		if (parsedMsg.getParams().size() == 1) {
			// TODO(hnoguchi): Info::getChannelByName(std::string name);を実装する。
			// TODO(hnoguchi): Channel::getModesStr();を実装する。
			if (parsedMsg.getParams()[0].getValue() == channelIt->getName()) {
				std::string		reply;
				if (channelIt->getModes() & kInviteOnly) {
					reply = Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getNickName(), parsedMsg.getParams()[0].getValue(), 'i', "");
				}
				if (channelIt->getModes() & kKey) {
					reply += Reply::rplFromName(user->getNickName());
					reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getNickName(), parsedMsg.getParams()[0].getValue(), 'k', channelIt->getKey());
				}
				if (channelIt->getModes() & kLimit) {
					reply += Reply::rplFromName(user->getNickName());
					std::stringstream	ss;
					ss << channelIt->getLimit();
					reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getNickName(), parsedMsg.getParams()[0].getValue(), 'l', ss.str());
				}
				if (channelIt->getModes() & kRestrictTopicSetting) {
					reply += Reply::rplFromName(user->getNickName());
					reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getNickName(), parsedMsg.getParams()[0].getValue(), 't', "");
				}
				return (reply);
			}
		}
		if (parsedMsg.getParams()[1].getValue().size() != 2) {
			return (Reply::errUnknownMode(kERR_UNKNOWNMODE, user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
		}
		if (parsedMsg.getParams()[1].getValue()[0] != '+' && parsedMsg.getParams()[1].getValue()[0] != '-') {
			return (Reply::errUnknownMode(kERR_UNKNOWNMODE,user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
		}
		unsigned long	pos = info->getConfig().getChannelModes().find(parsedMsg.getParams()[1].getValue()[1]);
		if (pos == std::string::npos) {
			return (Reply::errUnknownMode(kERR_UNKNOWNMODE,user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
		}
		// そのチャンネルで使用できるモードか確認する。
		std::string	msg = ":" + user->getNickName() + " MODE " + channelIt->getName() + " " + parsedMsg.getParams()[1].getValue();
		if (parsedMsg.getParams()[1].getValue()[0] == '+') {
			if (parsedMsg.getParams()[1].getValue()[1] == 'i') {
				// TODO(hnoguchi): Change prefix
				channelIt->setMode(kInviteOnly);
				msg += "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'k') {
				// keyフラグが既に立っているか確認する。
				if (channelIt->getModes() & kKey) {
					return (Reply::errKeySet(kERR_KEYSET, user->getNickName(), user->getNickName(), channelIt->getName()));
				}
				if (parsedMsg.getParams().size() < 3) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				}
				// TODO(hnoguchi): Validate string key
				channelIt->setKey(parsedMsg.getParams()[2].getValue());
				channelIt->setMode(kKey);
				msg += " " + parsedMsg.getParams()[2].getValue() + "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'l') {
				if (parsedMsg.getParams().size() < 3) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				}
				// TODO(hnoguchi): Validate Value
				channelIt->setLimit(std::atoi(parsedMsg.getParams()[2].getValue().c_str()));
				channelIt->setMode(kLimit);
				msg += " " + parsedMsg.getParams()[2].getValue() + "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'o') {
				if (parsedMsg.getParams().size() < 3) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				}
				std::vector<User*>::iterator	targetUserIt = const_cast<std::vector<User*> &>(channelIt->getMembers()).begin();
				for (; targetUserIt != channelIt->getMembers().end(); targetUserIt++) {
					if (parsedMsg.getParams()[2].getValue() == (*targetUserIt)->getNickName()) {
						break;
					}
				}
				if (targetUserIt == channelIt->getMembers().end()) {
					return (Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getNickName(), parsedMsg.getParams()[2].getValue(), channelIt->getName()));
				}
				channelIt->addOperator(*targetUserIt);
				msg += " " + parsedMsg.getParams()[2].getValue() + "\r\n";
				debugPrintSendMessage("SendMsg", msg);
				sendNonBlocking((*targetUserIt)->getFd(), msg.c_str(), msg.size());
				// TODO(hnoguchi): Check error
			} else if (parsedMsg.getParams()[1].getValue()[1] == 't') {
				channelIt->setMode(kRestrictTopicSetting);
				msg += "\r\n";
			}
		} else {
			if (parsedMsg.getParams()[1].getValue()[1] == 'i') {
				// TODO(hnoguchi): Change prefix
				channelIt->unsetMode(kInviteOnly);
				msg += "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'k') {
				channelIt->setKey("");
				channelIt->unsetMode(kKey);
				msg += "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'l') {
				channelIt->setLimit(0);
				channelIt->unsetMode(kLimit);
				msg += "\r\n";
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'o') {
				if (parsedMsg.getParams().size() < 3) {
					return (Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getNickName(), parsedMsg.getCommand()));
				}
				std::vector<User*>::iterator	targetUserIt = const_cast<std::vector<User*> &>(channelIt->getMembers()).begin();
				for (; targetUserIt != channelIt->getMembers().end(); targetUserIt++) {
					if (parsedMsg.getParams()[2].getValue() == (*targetUserIt)->getNickName()) {
						break;
					}
				}
				if (targetUserIt == channelIt->getMembers().end()) {
					return (Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getNickName(), parsedMsg.getParams()[2].getValue(), channelIt->getName()));
				}
				channelIt->eraseOperator(*targetUserIt);
				msg += parsedMsg.getParams()[2].getValue() + "\r\n";
				debugPrintSendMessage("SendMsg", msg);
				sendNonBlocking((*targetUserIt)->getFd(), msg.c_str(), msg.size());
				// TODO(hnoguchi): Check error
			} else if (parsedMsg.getParams()[1].getValue()[1] == 't') {
				channelIt->unsetMode(kRestrictTopicSetting);
				msg += "\r\n";
			}
		}
		debugPrintSendMessage("SendMsg", msg);
		sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
		return ("");
		// TODO(hnoguchi): Check error
	} catch (const std::exception& e) {
		std::cerr << RED << e.what() << END << std::endl;
		throw;
		// return (-1);
	}
}
