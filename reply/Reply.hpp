#ifndef REPLY_HPP
# define REPLY_HPP

#include <string>

// TODO(hnoguchi): welcome messageで使用するものは分ける？
enum kCmdReplyNum {
	kRPL_WELCOME = 1,
	kRPL_YOURHOST = 2,
	kRPL_CREATED = 3,
	kRPL_MYINFO = 4,
	kRPL_AWAY = 301,
	kRPL_TOPIC = 332
};

enum kErrReplyNum {
	kERR_NOSUCHNICK = 401,
	kERR_CANNOTSENDTOCHAN = 404,
	kERR_TOOMANYTARGETS = 407,
	kERR_NORECIPIENT = 411,
	kERR_NOTEXTTOSEND = 412,
	kERR_NOTOPLEVEL = 413,
	kERR_WILDTOPLEVEL = 414,
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
