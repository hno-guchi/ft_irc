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
	 static const std::string	cmdList_[];

 public:
	 explicit Execute(const Command &command);
	 ~Execute();

	bool	isCommand();
	int		exec(User* user, std::vector<User>* users, \
			std::vector<Channel>* channels);
};

#endif  // EXECUTE_HPP
