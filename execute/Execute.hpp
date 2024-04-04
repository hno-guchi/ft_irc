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
 public:
	 Execute();
	 ~Execute();

	// TODO(hnoguchi): コマンドのバリデーションは、Parser classで行うので必要ない？
	bool	isCommand(const std::string& command, const std::string* cmdList);
	// TODO(hnoguchi): 引数にConfig configを追加で渡す。
	int		exec(User* user, const Command& command, Info* info);
};

#endif  // EXECUTE_HPP
