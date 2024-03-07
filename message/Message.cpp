#include "./Message.hpp"
#include "../reply/Reply.hpp"
#include "../user/User.hpp"

Message::Message() : delimiter_("\r\n") {
	this->initializeCmdReplyMessageList();
	this->initializeErrReplyMessageList();
}


Message::~Message() {}

void	Message::initializeCmdReplyMessageList() {
	Reply	r;
	r.setNumeric("001");
	r.setMessage("Welcome to the Internet Relay Network <nick>!<user>@<host>");
	this->cmdReplyMsgList_.insert(std::make_pair(kRPL_WELCOME, r));

	r.setNumeric("002");
	r.setMessage("Your host is <servername>, running version <ver>");
	this->cmdReplyMsgList_.insert(std::make_pair(kRPL_YOURHOST, r));

	r.setNumeric("003");
	r.setMessage("This server was created <date>");
	this->cmdReplyMsgList_.insert(std::make_pair(kRPL_CREATED, r));

	r.setNumeric("004");
	r.setMessage("<servername> <version> <available user modes> <available channel modes>");
	this->cmdReplyMsgList_.insert(std::make_pair(kRPL_MYINFO, r));

	r.setNumeric("332");
	r.setMessage("<channel> :<topic>");
	this->cmdReplyMsgList_.insert(std::make_pair(kRPL_TOPIC, r));

	return;
}

void	Message::initializeErrReplyMessageList() {
	Reply	r;
	r.setNumeric("421");
	r.setMessage("<command> :Unknown command");
	this->errReplyMsgList_.insert(std::make_pair(kERR_UNKNOWNCOMMAND, r));

	r.setNumeric("461");
	r.setMessage("<command> :Not enough parameters");
	this->errReplyMsgList_.insert(std::make_pair(kERR_NEEDMOREPARAMS, r));

	return;
}

std::string	Message::createMessage(int num, const User& user) {
	if (num <= 0) {
		return ("");
	}
	// TODO(hnoguchi): Check num.
	std::string	msg = ":";
	msg += user.getNickName();
	msg += " ";
	if (num < 100  || (num >= 200 && num < 400)) {
		msg += this->cmdReplyMsgList_[static_cast<kCmdReplyNum>(num)].getNumeric();
		msg += " ";
		msg += this->cmdReplyMsgList_[static_cast<kCmdReplyNum>(num)].getMessage();
		msg += this->delimiter_;
	} else if (num >= 400 && num < 600) {
		msg = this->errReplyMsgList_[static_cast<kErrReplyNum>(num)].getNumeric();
		msg += " ";
		msg += this->errReplyMsgList_[static_cast<kErrReplyNum>(num)].getMessage();
		msg += this->delimiter_;
	}
	return (msg);
}
