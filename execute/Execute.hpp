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
	 void	cmdPass(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdNick(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdUser(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdUserMode(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdOper(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdQuit(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdPong(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdNotice(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdPrivmsg(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdJoin(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdPart(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdChannelMode(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdInvite(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdKick(User* user, const ParsedMsg& parsedMsg, Info* info);
	 void	cmdTopic(User* user, const ParsedMsg& parsedMsg, Info* info);

 public:
	 Execute();
	 ~Execute();

	void	registerUser(User* user, const ParsedMsg& parsedMsg, Info* info);
	void	exec(User* user, const ParsedMsg& parsedMsg, Info* info);
};

#endif  // EXECUTE_HPP
