#ifndef REPLY_HPP
# define REPLY_HPP

#include <string>

enum kCmdReplyNum {
	kNotSet = 0,
	kRPL_WELCOME = 1,
	kRPL_YOURHOST = 2,
	kRPL_CREATED = 3,
	kRPL_MYINFO = 4,
	kRPL_TOPIC = 332
};

enum kErrReplyNum {
	kERR_UNKNOWNCOMMAND = 421,
	kERR_NEEDMOREPARAMS = 461
};

class Reply {
 private:
	 std::string	message_;
	 std::string	numeric_;
 public:
	 Reply();
	 ~Reply();
	 // SETTER
	 void	setMessage(const std::string& message);
	 void	setNumeric(const std::string& num);
	 // GETTER
	 const std::string&	getMessage() const;
	 const std::string&	getNumeric() const;
	 // debug
	 void	printReply() const;
};

#endif  // REPLY_HPP
