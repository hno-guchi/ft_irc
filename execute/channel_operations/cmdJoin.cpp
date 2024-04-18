/*
 * Command   : JOIN
 * Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS  "<command> :Not enough parameters"
 *             474 ERR_BANNEDFROMCHAN  "<channel> :Cannot join channel (+b)"
 *             473 ERR_INVITEONLYCHAN  "<channel> :Cannot join channel (+i)"
 *             475 ERR_BADCHANNELKEY   "<channel> :Cannot join channel (+k)"
 *             471 ERR_CHANNELISFULL   "<channel> :Cannot join channel (+l)"
 *             476 ERR_BADCHANMASK     "<channel> :Bad Channel Mask"
 *             403 ERR_NOSUCHCHANNEL   "<channel name> :No such channel"
 *             405 ERR_TOOMANYCHANNELS "<target> :<error code> recipients. <abort message>"
 *             407 ERR_TOOMANYTARGETS  "<target> :<error code> recipients. <abort message>"
 *             437 ERR_UNAVAILRESOURCE "<nick/channel> :Nick/channel is temporarily unavailable"
 *             332 RPL_TOPIC           "<channel> :<topic>"
 *
 * Examples:
 *    JOIN #foobar                ; Command to join channel #foobar.
 *    JOIN &foo fubar             ; Command to join channel &foo using key "fubar".
 *    JOIN #foo,&bar fubar        ; Command to join channel #foo using key "fubar" and &bar using no key.
 *    JOIN #foo,#bar fubar,foobar ; Command to join channel #foo using key "fubar", and channel #bar using key "foobar".
 *    JOIN #foo,#bar              ; Command to join channels #foo and #bar.
 *    JOIN 0                      ; Leave all currently joined channels.
 *    :WiZ!jto@tolsun.oulu.fi JOIN #Twilight_zone
 *                                ; JOIN message from WiZ on channel #Twilight_zone
 */

#include <iostream>
#include <vector>
#include "../Execute.hpp"
#include "../../error/error.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Info.hpp"
#include "../../server/Server.hpp"
#include "../../reply/Reply.hpp"

int	Execute::cmdJoin(User* user, const ParsedMessage& parsedMsg, Info* info) {
	// channelを探す
	try {
		for (std::vector<Channel>::iterator it = const_cast<std::vector<Channel> &>(info->getChannels()).begin(); it != const_cast<std::vector<Channel> &>(info->getChannels()).end(); it++) {
			// あれば、追加して、リプライナンバーを返す
			if (parsedMsg.getParams()[0].getValue() == it->getName()) {
				it->addMember(user);
				std::string	msg = ":" + user->getNickName() + " JOIN " + it->getName() + "\r\n";
				debugPrintSendMessage("SendMsg", msg);
				sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
				return (0);
			}
		}
		// なければchannelを作成して、ユーザを追加して、リプライナンバーを返す
		info->addChannel(Channel(parsedMsg.getParams()[0].getValue()));
		for (std::vector<Channel>::iterator it = const_cast<std::vector<Channel> &>(info->getChannels()).begin(); it != const_cast<std::vector<Channel> &>(info->getChannels()).end(); it++) {
			// あれば、追加して、リプライナンバーを返す
			if (parsedMsg.getParams()[0].getValue() == it->getName()) {
				it->addMember(user);
				it->addOperator(user);
				std::string	msg = ":" + user->getNickName() + " JOIN " + it->getName() + "\r\n";
				msg += ":" + info->getConfig().getServerName() + " MODE " + it->getName() + " +o " + user->getNickName() + "\r\n";
				// msg += ":" + user->getNickName() + " MODE " + it->getName() + " :+o\r\n";
				debugPrintSendMessage("SendMsg", msg);
				sendNonBlocking(user->getFd(), msg.c_str(), msg.size());
				return (0);
			}
		}
	} catch (std::exception& e) {
		std::cerr << RED << e.what() << END << std::endl;
	}
	return (0);
}
