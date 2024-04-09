#include "./Reply.hpp"
#include "../parser/Parser.hpp"
#include "../error/error.hpp"

// メッセージ作成に必要な情報源
// <nick>, <user>, <host>: メッセージ本文
// <servername>, <ver>, <date>, <available user modes>, <available channel modes>: Config class
// 001	RPL_WELCOME				"Welcome to the Internet Relay Network <nick>!<user>@<host>"
// 002	RPL_YOURHOST			"Your host is <servername>, running version <ver>"
// 003	RPL_CREATED				"This server was created <date>"
// 004	RPL_MYINFO				"<servername> <version> <available user modes> <available channel modes>"

// ERROR
// None.

// PING
// 409	ERR_NOORIGIN			":No origin specified"
// 402	ERR_NOSUCHSERVER		"<server name> :No such server"

// PONG
// 409	ERR_NOORIGIN			":No origin specified"
// 402	ERR_NOSUCHSERVER		"<server name> :No such server"

// PRIVMSG
// 411	ERR_NORECIPIENT			":No recipient given (<command>)"
// 412	ERR_NOTEXTTOSEND		":No text to send"
// 404	ERR_CANNOTSENDTOCHAN	"<channel name> :Cannot send to channel"
// 413	ERR_NOTOPLEVEL			"<mask> :No toplevel domain specified"
// 414	ERR_WILDTOPLEVEL		"<mask> :Wildcard in toplevel domain"
// 407	ERR_TOOMANYTARGETS		"<target> :<error code> recipients. <abort message>"
// 401	ERR_NOSUCHNICK			"<nickname> :No such nick/channel"
// 301	RPL_AWAY				"<nick> :<away message>"
 
// INVITE
// 461	ERR_NEEDMOREPARAMS		"<command> :Not enough parameters"
// 401	ERR_NOSUCHNICK			"<nickname> :No such nick/channel"
// 442	ERR_NOTONCHANNEL		"<channel> :You're not on that channel"
// 443	ERR_USERONCHANNEL		"<user> <channel> :is already on channel"
// 482	ERR_CHANOPRIVSNEEDED	"<channel> :You're not channel operator"
// 341	RPL_INVITING			"<channel> <nick>"
// 301	RPL_AWAY				"<nick> :<away message>"

// JOIN
// 461	ERR_NEEDMOREPARAMS		"<command> :Not enough parameters"
// 474	ERR_BANNEDFROMCHAN		"<channel> :Cannot join channel (+b)"
// 473	ERR_INVITEONLYCHAN		"<channel> :Cannot join channel (+i)"
// 475	ERR_BADCHANNELKEY		"<channel> :Cannot join channel (+k)"
// 471	ERR_CHANNELISFULL		"<channel> :Cannot join channel (+l)"
// 476	ERR_BADCHANMASK			"<channel> :Bad Channel Mask"
// 403	ERR_NOSUCHCHANNEL		"<channel name> :No such channel"
// 405	ERR_TOOMANYCHANNELS		"<target> :<error code> recipients. <abort message>"
// 407	ERR_TOOMANYTARGETS		"<target> :<error code> recipients. <abort message>"
// 437	ERR_UNAVAILRESOURCE		"<nick/channel> :Nick/channel is temporarily unavailable"
// 332	RPL_TOPIC				"<channel> :<topic>"

// KICK
// 461	ERR_NEEDMOREPARAMS		"<command> :Not enough parameters"
// 403	ERR_NOSUCHCHANNEL		"<channel name> :No such channel"
// 476	ERR_BADCHANMASK			"<channel> :Bad Channel Mask"
// 482	ERR_CHANOPRIVSNEEDED	"<channel> :You're not channel operator"
// 441	ERR_USERNOTINCHANNEL	"<nick> <channel> :They aren't on that channel"
// 442	ERR_NOTONCHANNEL		"<channel> :You're not on that channel"

// MODE
// 461	ERR_NEEDMOREPARAMS		"<command> :Not enough parameters"
// 467	ERR_KEYSET				"<channel> :Channel key already set"
// 477	ERR_NOCHANMODES			"<channel> :Channel doesn't support modes"
// 482	ERR_CHANOPRIVSNEEDED	"<channel> :You're not channel operator"
// 441	ERR_USERNOTINCHANNEL	"<nick> <channel> :They aren't on that channel"
// 472	ERR_UNKNOWNMODE			"<char> :is unknown mode char to me for <channel>"
// 324	RPL_CHANNELMODEIS		"<channel> <mode> <mode params>"
// 367	RPL_BANLIST				"<channel> <banmask>"
// 368	RPL_ENDOFBANLIST		"<channel> :End of channel ban list"
// 348	RPL_EXCEPTLIST			"<channel> <exceptionmask>"
// 349	RPL_ENDOFEXCEPTLIST		"<channel> :End of channel exception list"
// 346	RPL_INVITELIST			"<channel> <invitemask>"
// 347	RPL_ENDOFINVITELIST		"<channel> :End of channel invite list"
// 325	RPL_UNIQOPIS			"<channel> <nickname>"

// PART
// 461	ERR_NEEDMOREPARAMS		"<command> :Not enough parameters"
// 403	ERR_NOSUCHCHANNEL		"<channel name> :No such channel"
// 442	ERR_NOTONCHANNEL		"<channel> :You're not on that channel"

// TOPIC
// 461	ERR_NEEDMOREPARAMS		"<command> :Not enough parameters"
// 442	ERR_NOTONCHANNEL		"<channel> :You're not on that channel"
// 331	RPL_NOTOPIC				"<channel> :No topic is set"
// 332	RPL_TOPIC				"<channel> :<topic>"
// 482	ERR_CHANOPRIVSNEEDED	"<channel> :You're not channel operator"
// 477	ERR_NOCHANMODES			"<channel> :Channel doesn't support modes"

// MODE
// 461	ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
// 502	ERR_USERSDONTMATCH   ":Cannot change mode for other users"
// 501	ERR_UMODEUNKNOWNFLAG ":Unknown MODE flag"
// 221	RPL_UMODEIS          "<user mode string>"

// NICK
// 431	ERR_NONICKNAMEGIVEN  ":No nickname given"
// 432	ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
// 433	ERR_NICKNAMEINUSE    "<nick> :Nickname is already in use"
// 436	ERR_NICKCOLLISION    "<nick> :Nickname collision KILL from <user>@<host>"
// 437	ERR_UNAVAILRESOURCE  "<nick/channel> :Nick/channel is temporarily unavailable"
// 484	ERR_RESTRICTED       ":Your connection is restricted!"

// OPER
// 461	ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
// 381	RPL_YOUREOPER        ":You are now an IRC operator"
// 491	ERR_NOOPERHOST       ":No O-lines for your host"
// 464	ERR_PASSWDMISMATCH   ":Password incorrect"

// PASS
// 461	ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
// 462	ERR_ALREADYREGISTRED ":Unauthorized command (already registered)"

// QUIT
// None.

// USER
// 461	ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
// 462	ERR_ALREADYREGISTRED ":Unauthorized command (already registered)"

// 451	ERR_NOTREGISTERED	":You have not registered"
// 421	ERR_UNKNOWNCOMMAND	"<command> :Unknown command"

// CONSTRUCTOR
// DESTRUCTOR
Reply::Reply() : delimiter_("\r\n") {}
Reply::~Reply() {}

// ":<sender prefix> <the three digit numeric> <the target of the reply>\r\n"
std::string	Reply::createMessage(int num, const User& user, const Info& info, const ParsedMessage& parsedMsg) {
	// TODO(hnoguchi): userはいらない？
	(void)user;
	if (num <= 0) {
		return ("");
	}
	// TODO(hnoguchi): Check num.
	// std::string	msg = ":" + user.getNickName();
	std::string	msg = ":" + info.getConfig().getServerName() + " ";
	if (num < 100  || (num >= 200 && num < 400)) {
		// msg += this->cmdReplyMsgList_[static_cast<kCmdReplyNum>(num)].getNumeric();
		msg += " ";
		// msg += this->cmdReplyMsgList_[static_cast<kCmdReplyNum>(num)].getMessage();
	} else if (num >= 400 && num < 600) {
		if (num == kERR_NOSUCHNICK) {
			msg += "401 :" + parsedMsg.getParams()[0].getValue() \
					+ " :No such nick/channel";
		} else if (num == kERR_NOSUCHSERVER) {
			msg += "402 :" + parsedMsg.getParams()[0].getValue() + " :No such server";
		} else if (num == kERR_UNKNOWNCOMMAND) {
			msg += "421 :" + parsedMsg.getCommand() + " :Unknown command";
		} else if (num == kERR_NOTREGISTERED) {
			msg += "451 :You have not registered";
		}
	}
	msg += this->delimiter_;
	return (msg);
}


// SETTER
// void Reply::setMessage(const std::string& message) {
// 	if (message.empty()) {
// 		printErrorMessage("Reply::setMessage is empty argument.");
// 		return;
// 	}
// 	this->message_ = message;
// 	return;
// }

// void Reply::setNumeric(const std::string& num) {
// 	if (num.empty()) {
// 		printErrorMessage("Reply::setNumeric is empty argument.");
// 		return;
// 	}
// 	this->numeric_ = num;
// 	return;
// }

// GETTER
// const std::string&	Reply::getMessage() const {
// 	return (this->message_);
// }

// const std::string&	Reply::getNumeric() const {
// 	return (this->numeric_);
// }

// debug
// void	Reply::printReply() const {
// 	std::cout << "[" << this->numeric_ << "]: [" \
// 		<< this->message_ << "]" << std::endl;
// }
