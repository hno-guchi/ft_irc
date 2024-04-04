#ifndef COMMAND_HPP
# define COMMAND_HPP

int	pong(User* user, const Command& command, Info* info);
int	notice(User* user, const Command& command, Info* info);
int	privmsg(User* user, const Command& command, Info* info);
int	join(User* user, const Command& command, Info* info);

#endif
