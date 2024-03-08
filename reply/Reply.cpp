#include "./Reply.hpp"
#include "../error/error.hpp"

// CONSTRUCTOR
// DESTRUCTOR
Reply::Reply() {}
Reply::~Reply() {}

// SETTER
void Reply::setMessage(const std::string& message) {
	if (message.empty()) {
		printErrorMessage("Reply::setMessage is empty argument.");
		return;
	}
	this->message_ = message;
	return;
}

void Reply::setNumeric(const std::string& num) {
	if (num.empty()) {
		printErrorMessage("Reply::setNumeric is empty argument.");
		return;
	}
	this->numeric_ = num;
	return;
}

// GETTER
const std::string&	Reply::getMessage() const {
	return (this->message_);
}

const std::string&	Reply::getNumeric() const {
	return (this->numeric_);
}

// debug
void	Reply::printReply() const {
	std::cout << "[" << this->numeric_ << "]: [" \
		<< this->message_ << "]" << std::endl;
}

#ifdef DEBUG
#include <map>

static void printReplyList(const std::map<kCmdReplyNum, Reply>& list) {
	std::cout << std::endl;
	list.at(kRPL_WELCOME).printReply();
	list.at(kRPL_YOURHOST).printReply();
	list.at(kRPL_CREATED).printReply();
	list.at(kRPL_MYINFO).printReply();
	std::cout << std::endl;
}

static void	insertReplyList(std::map<kCmdReplyNum, Reply>* list) {
	Reply	r;
	r.setNumeric("001");
	r.setMessage("Welcome to the Internet Relay Network <nick>!<user>@<host>");
	list->insert(std::make_pair(kRPL_WELCOME, r));

	r.setNumeric("002");
	r.setMessage("Your host is <servername>, running version <ver>");
	list->insert(std::make_pair(kRPL_YOURHOST, r));

	r.setNumeric("003");
	r.setMessage("This server was created <date>");
	list->insert(std::make_pair(kRPL_CREATED, r));

	r.setNumeric("004");
	r.setMessage("<servername> <version> <available user modes> <available channel modes>");
	list->insert(std::make_pair(kRPL_MYINFO, r));

	r.setNumeric("301");
	r.setMessage("<nick> :<away message>");
	list->insert(std::make_pair(kRPL_MYINFO, r));

	r.setNumeric("332");
	r.setMessage();
	list->insert(std::make_pair(kRPL_MYINFO, r));
}

int	main() {
	std::map<kCmdReplyNum, Reply> list;

	insertReplyList(&list);
	printReplyList(list);
#ifdef LEAKS
	system("leaks -q reply");
# endif  // LEAKS
	return (0);
}
#endif  // DEBUG
