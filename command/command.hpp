#ifndef COMMAND_HPP
# define COMMAND_HPP

int	pong(User* user, const ParsedMessage& parsedMsg, Info* info);
int	notice(User* user, const ParsedMessage& parsedMsg, Info* info);
int	privmsg(User* user, const ParsedMessage& parsedMsg, Info* info);
int	join(User* user, const ParsedMessage& parsedMsg, Info* info);

#endif
