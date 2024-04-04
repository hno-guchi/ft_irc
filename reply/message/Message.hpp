#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <map>
#include <string>
#include "../reply/Reply.hpp"
#include "../user/User.hpp"

class Message {
 private:
	 const std::string				delimiter_;
	 std::map<kCmdReplyNum, Reply>	cmdReplyMsgList_;
	 std::map<kErrReplyNum, Reply>	errReplyMsgList_;

	 void	initializeCmdReplyMessageList();
	 void	initializeErrReplyMessageList();

 public:
	Message();
	~Message();

	std::string	createMessage(int num, const User& user);
};

#endif  // MESSAGE_HPP
