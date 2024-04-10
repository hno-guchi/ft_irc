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
	 int	cmdNick(User* user, const ParsedMessage& parsedMsg, Info* info);
	 int	cmdUser(User* user, const ParsedMessage& parsedMsg, Info* info);
	 int	cmdPong(User* user, const ParsedMessage& parsedMsg, Info* info);
	 int	cmdNotice(User* user, const ParsedMessage& parsedMsg, Info* info);
	 int	cmdPrivmsg(User* user, const ParsedMessage& parsedMsg, Info* info);
	 int	cmdJoin(User* user, const ParsedMessage& parsedMsg, Info* info);
 public:
	 Execute();
	 ~Execute();

	// TODO(hnoguchi): コマンドのバリデーションは、Parser classで行うので必要ない。コマンドで条件分岐を行い、バリデーションを実行する。
	bool	isCommand(const std::string& command, const std::string* cmdList);
	int		registerUser(User* user, const ParsedMessage& parsedMsg, Info* info);
	int		exec(User* user, const ParsedMessage& parsedMsg, Info* info);
};

#endif  // EXECUTE_HPP
