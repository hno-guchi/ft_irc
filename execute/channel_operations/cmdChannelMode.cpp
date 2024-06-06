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
void	Execute::cmdChannelMode(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		std::vector<Channel*>::iterator	channelIt = info->findChannel(parsedMsg.getParams()[0].getValue());
		if (channelIt == info->getChannels().end()) {
			std::string	reply = Reply::rplFromName(info->getServerName());
			reply += Reply::errNoSuchChannel(kERR_NOSUCHCHANNEL, user->getPrefixName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		if (parsedMsg.getParams().size() == 1) {
			// TODO(hnoguchi): Info::getChannelByName(std::string name);を実装する。
			if ((*channelIt)->getModes() & kInviteOnly) {
				std::string	reply = Reply::rplFromName(info->getServerName());
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getPrefixName(), parsedMsg.getParams()[0].getValue(), 'i', "");
				Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			if ((*channelIt)->getModes() & kKeySet) {
				std::string	reply = Reply::rplFromName(user->getNickName());
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getPrefixName(), parsedMsg.getParams()[0].getValue(), 'k', (*channelIt)->getKey());
				Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			if ((*channelIt)->getModes() & kLimit) {
				std::string			reply = Reply::rplFromName(user->getNickName());
				std::stringstream	ss;
				ss << (*channelIt)->getLimit();
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getPrefixName(), parsedMsg.getParams()[0].getValue(), 'l', ss.str());
				Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			if ((*channelIt)->getModes() & kRestrictTopicSetting) {
				std::string	reply = Reply::rplFromName(user->getNickName());
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getPrefixName(), parsedMsg.getParams()[0].getValue(), 't', "");
				Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			for (std::vector<User*>::const_iterator it = (*channelIt)->getOperators().begin(); it != (*channelIt)->getOperators().end(); it++) {
				std::string	reply = Reply::rplFromName(user->getNickName());
				reply += Reply::rplChannelModeIs(kRPL_CHANNELMODEIS, user->getPrefixName(), parsedMsg.getParams()[0].getValue(), 'o', (*it)->getNickName());
				Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			}
			// std::string	reply = Reply::rplFromName(user->getNickName());
			// reply += Reply::errNoChanModes(kERR_NOCHANMODES, user->getPrefixName(), user->getNickName(), parsedMsg.getParams()[0].getValue());
			// Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// channel operatorか確認する。
		std::string	reply = Reply::rplFromName(info->getServerName());
		if (!(*channelIt)->isOperator(user->getNickName())) {
			reply += Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getPrefixName(), user->getNickName(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		unsigned long	pos = info->getChannelModes().find(parsedMsg.getParams()[1].getValue()[1]);
		if (pos == std::string::npos) {
			reply += Reply::errUnknownMode(kERR_UNKNOWNMODE, user->getPrefixName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue());
			Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
			return;
		}
		// そのチャンネルで使用できるモードか確認する。
		std::string	msg = ":" + user->getPrefixName() + " MODE " + (*channelIt)->getName() + " " + parsedMsg.getParams()[1].getValue();
		if (parsedMsg.getParams()[1].getValue()[0] == '+') {
			if (parsedMsg.getParams()[1].getValue()[1] == 'i') {
				// TODO(hnoguchi): Change prefix
				(*channelIt)->setMode(kInviteOnly);
				msg += Reply::getDelimiter();
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'k') {
				// keyフラグが既に立っているか確認する。
				if ((*channelIt)->getModes() & kKeySet) {
					reply += Reply::errKeySet(kERR_KEYSET, user->getPrefixName(), user->getNickName(), (*channelIt)->getName());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				if (parsedMsg.getParams().size() < 3) {
					reply += Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getPrefixName(), parsedMsg.getCommand());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				// TODO(hnoguchi): Validate string key
				(*channelIt)->setKey(parsedMsg.getParams()[2].getValue());
				(*channelIt)->setMode(kKeySet);
				msg += " " + parsedMsg.getParams()[2].getValue() + Reply::getDelimiter();
				// std::cerr << "key: " << (*channelIt)->getKey() << std::endl;
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'l') {
				if (parsedMsg.getParams().size() < 3) {
					reply += Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getPrefixName(), parsedMsg.getCommand());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				// TODO(hnoguchi): Validate Value
				(*channelIt)->setLimit(std::atoi(parsedMsg.getParams()[2].getValue().c_str()));
				(*channelIt)->setMode(kLimit);
				msg += " " + parsedMsg.getParams()[2].getValue() + Reply::getDelimiter();
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'o') {
				if (parsedMsg.getParams().size() < 3) {
					reply += Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getPrefixName(), parsedMsg.getCommand());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				if (!(*channelIt)->isMember(parsedMsg.getParams()[2].getValue())) {
					reply += Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getPrefixName(), parsedMsg.getParams()[2].getValue(), (*channelIt)->getName());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				(*channelIt)->pushBackOperator(*info->findUser(parsedMsg.getParams()[2].getValue()));
				msg += " " + parsedMsg.getParams()[2].getValue() + Reply::getDelimiter();
				// Server::sendNonBlocking(*((*targetUserIt)->getFd()), msg.c_str(), msg.size());
			} else if (parsedMsg.getParams()[1].getValue()[1] == 't') {
				(*channelIt)->setMode(kRestrictTopicSetting);
				msg += Reply::getDelimiter();
			}
		} else if (parsedMsg.getParams()[1].getValue()[0] == '-') {
			if (parsedMsg.getParams()[1].getValue()[1] == 'i') {
				// TODO(hnoguchi): Change prefix
				(*channelIt)->unsetMode(kInviteOnly);
				msg += Reply::getDelimiter();
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'k') {
				(*channelIt)->setKey("");
				(*channelIt)->unsetMode(kKeySet);
				msg += Reply::getDelimiter();
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'l') {
				(*channelIt)->setLimit(0);
				(*channelIt)->unsetMode(kLimit);
				msg += Reply::getDelimiter();
			} else if (parsedMsg.getParams()[1].getValue()[1] == 'o') {
				if (parsedMsg.getParams().size() < 3) {
					reply += Reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user->getPrefixName(), parsedMsg.getCommand());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				if ((*channelIt)->isMember(parsedMsg.getParams()[2].getValue())) {
					reply += Reply::errUserNotInChannel(kERR_USERNOTINCHANNEL, user->getPrefixName(), parsedMsg.getParams()[2].getValue(), (*channelIt)->getName());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				if ((*channelIt)->isOperator(parsedMsg.getParams()[2].getValue())) {
					reply += Reply::errChanOprivsNeeded(kERR_CHANOPRIVSNEEDED, user->getPrefixName(), parsedMsg.getParams()[2].getValue(), (*channelIt)->getName());
					Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
					return;
				}
				(*channelIt)->eraseOperator(*info->findUser(parsedMsg.getParams()[2].getValue()));
				msg += parsedMsg.getParams()[2].getValue() + Reply::getDelimiter();
				Server::sendNonBlocking((*info->findUser(parsedMsg.getParams()[2].getValue()))->getFd(), msg.c_str(), msg.size());
			} else if (parsedMsg.getParams()[1].getValue()[1] == 't') {
				(*channelIt)->unsetMode(kRestrictTopicSetting);
				msg += Reply::getDelimiter();
			}
		}
		for (std::vector<User*>::const_iterator it = (*channelIt)->getMembers().begin(); it != (*channelIt)->getMembers().end(); it++) {
			Server::sendNonBlocking((*it)->getFd(), msg.c_str(), msg.size());
		}
	} catch (const std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
