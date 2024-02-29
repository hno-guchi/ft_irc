#include "./Reply.hpp"
#include "../error/error.hpp"

// 001  "Welcome to the Internet Relay Network <nick>!<user>@<host>"
// 002  "Your host is <servername>, running version <ver>"
// 003  "This server was created <date>"
// 004  "<servername> <version> <available user modes> <available channel modes>"

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
// 001  "Welcome to the Internet Relay Network <nick>!<user>@<host>"
// 002  "Your host is <servername>, running version <ver>"
// 003  "This server was created <date>"
// 004  "<servername> <version> <available user modes> <available channel modes>"

#include <map>

static void printReplyList(const std::map<kReplyNum, Reply>& list) {
	std::cout << std::endl;
	list.at(kRPL_WELCOME).printReply();
	list.at(kRPL_YOURHOST).printReply();
	list.at(kRPL_CREATED).printReply();
	list.at(kRPL_MYINFO).printReply();
	// for (std::map<kReplyNum, Reply>::const_iterator it = list.begin(); \
	// 		it != list.end(); it++) {
	// 	it->second.printReply();
	// }
	std::cout << std::endl;
}

static void	insertReplyList(std::map<kReplyNum, Reply>* list) {
	Reply	r1;
	r1.setNumeric("001");
	r1.setMessage("Welcome to the Internet Relay Network <nick>!<user>@<host>");
	list->insert(std::make_pair(kRPL_WELCOME, r1));

	Reply	r2;
	r2.setNumeric("002");
	r2.setMessage("Your host is <servername>, running version <ver>");
	list->insert(std::make_pair(kRPL_YOURHOST, r2));

	Reply	r3;
	r3.setNumeric("003");
	r3.setMessage("This server was created <date>");
	list->insert(std::make_pair(kRPL_CREATED, r3));

	Reply	r4;
	r4.setNumeric("004");
	r4.setMessage("<servername> <version> <available user modes> <available channel modes>");
	list->insert(std::make_pair(kRPL_MYINFO, r4));
}

int	main() {
	std::map<kReplyNum, Reply> list;

	insertReplyList(&list);
	printReplyList(list);
#ifdef LEAKS
	system("leaks -q reply");
# endif  // LEAKS
	return (0);
}
