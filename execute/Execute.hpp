#ifndef EXECUTE_HPP
# define EXECUTE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../server/Info.hpp"
#include "../parser/Parser.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"

class Execute {
 private:
	 std::string	cmdPass(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdNick(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdUser(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdUserMode(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdOper(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdQuit(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdPong(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdNotice(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdPrivmsg(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdJoin(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdPart(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdChannelMode(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdInvite(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdKick(User* user, const ParsedMessage& parsedMsg, Info* info);
	 std::string	cmdTopic(User* user, const ParsedMessage& parsedMsg, Info* info);
 public:
	 Execute();
	 ~Execute();

	// TODO(hnoguchi): コマンドのバリデーションは、Parser classで行うので必要ない。コマンドで条件分岐を行い、バリデーションを実行する。
	bool			isCommand(const std::string& command, const std::string* cmdList);
	std::string		registerUser(User* user, const ParsedMessage& parsedMsg, Info* info);
	std::string		exec(User* user, const ParsedMessage& parsedMsg, Info* info);
};

#endif  // EXECUTE_HPP
