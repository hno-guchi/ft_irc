/*
 *    Command: QUIT
 * Parameters: [ <Quit Message> ]
 *
 * Numeric Replies:
 *             None.
 *
 * Example:
 *    QUIT :Gone to have lunch
 *             ; Preferred message format.
 *
 *    :syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch
 *             ; User syrk has quit IRC to have lunch.
 */

#include <vector>
#include "../Execute.hpp"
#include "../../debug/debug.hpp"
#include "../../user/User.hpp"
#include "../../parser/Parser.hpp"
#include "../../server/Server.hpp"
#include "../../server/Info.hpp"
#include "../../reply/Reply.hpp"

void	Execute::cmdQuit(User* user, const ParsedMsg& parsedMsg, Info* info) {
	try {
		for (std::vector<Channel*>::const_iterator it = info->getChannels().begin(); it != info->getChannels().end(); ++it) {
			if ((*it)->isMember(user->getNickName())) {
				std::vector<Channel*>::iterator channelIt = info->findChannel((*it)->getName());
				(*channelIt)->eraseMember(user);
				(*channelIt)->eraseInvited(user);
				(*channelIt)->eraseOperator(user);
				std::string	message = ":" + user->getPrefixName() + " QUIT ";
				if (parsedMsg.getParams().size() > 0) {
					message += parsedMsg.getParams()[0].getValue();
				} else {
					message += ":Client Quit";
				}
				message += Reply::getDelimiter();
				for (std::vector<User*>::const_iterator it = (*channelIt)->getMembers().begin(); it != (*channelIt)->getMembers().end(); ++it) {
					Server::sendNonBlocking((*it)->getFd(), message.c_str(), message.size());
				}
				if ((*channelIt)->getMembers().size() == 0) {
					info->eraseChannel(info->findChannel((*channelIt)->getName()));
				}
				break;
			}
		}
		std::string	reply = ":" + user->getPrefixName() + " ERROR ";
		if (parsedMsg.getParams().size() > 0) {
			reply += parsedMsg.getParams()[0].getValue();
		} else {
			reply += ":Client Quit";
		}
		reply += Reply::getDelimiter();
		Server::sendNonBlocking(user->getFd(), reply.c_str(), reply.size());
		info->eraseUser(info->findUser(user->getFd()));
	} catch (const std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		throw;
	}
}
