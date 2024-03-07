#ifndef EXECUTE_HPP
# define EXECUTE_HPP

#include <iostream>
#include <string>
#include "../parser/Parser.hpp"
#include "../user/User.hpp"

class Execute {
 private:
	 const Command&				command_;
	 static const std::string	cmdList_[];

 public:
	 explicit Execute(const Command &command);
	 ~Execute();

	bool	isCommand();
	int		exec(User* user);
};

#endif  // EXECUTE_HPP
