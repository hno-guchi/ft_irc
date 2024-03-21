#ifndef EXECUTE_HPP
# define EXECUTE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../parser/Parser.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"

class Execute {
 private:
	 // TODO(hnoguchi): 参照渡しは、よくない気がする。
	 const Command&				command_;
	 // TODO(hnoguchi): Config classで定義する。
	 static const std::string	cmdList_[];

 public:
	 explicit Execute(const Command &command);
	 ~Execute();

	// TODO(hnoguchi): コマンドのバリデーションは、Parser classで行うので必要ない？
	bool	isCommand();
	// TODO(hnoguchi): 引数にConfig configを追加で渡す。
	int		exec(User* user, std::vector<User>* users, \
			std::vector<Channel>* channels);
};

#endif  // EXECUTE_HPP
