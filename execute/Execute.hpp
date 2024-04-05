#ifndef EXECUTE_HPP
# define EXECUTE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../server/Info.hpp"
#include "../parser/Parser.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"

// TODO(hnoguchi): Execute classのメンバ関数に、各コマンドの処理を実装する。
class Execute {
// private:
// 	int	pong(User* user, const Command& command, Info* info);
// 	int	notice(User* user, const Command& command, Info* info);
// 	int	privmsg(User* user, const Command& command, Info* info);
// 	int	join(User* user, const Command& command, Info* info);
 public:
	 Execute();
	 ~Execute();

	// TODO(hnoguchi): コマンドのバリデーションは、Parser classで行うので必要ない。コマンドで条件分岐を行い、バリデーションを実行する。
	bool	isCommand(const std::string& command, const std::string* cmdList);
	// TODO(hnoguchi): 引数にConfig configを追加で渡す。
	int		exec(User* user, const ParsedMessage& parsedMsg, Info* info);
};

#endif  // EXECUTE_HPP
