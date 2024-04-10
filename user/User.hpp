#ifndef USER_HPP
# define USER_HPP

#include <iostream>
#include <string>
#include "../color.hpp"

enum kUserMode {
	kAway = (1 << 0),
	kOperator = (1 << 1),
	kRestrict = (1 << 2),
	kChannelOperator = (1 << 3)
};

enum kExecCommand {
	kPassCommand = (1 << 0),
	kNickCommand = (1 << 1),
	kUserCommand = (1 << 2),
	kExecAllCmd = (kNickCommand | kUserCommand)
	// kExecAllCmd = (kPassCommand | kNickCommand | kUserCommand)
	// kExecAllCmdExceptPass = (kNickCommand | kUserCommand)
};

class User {
 private:
	 std::string	nickName_;
	 std::string	userName_;
	 std::string	hostName_;
	 std::string	serverName_;
	 std::string	realName_;
	 int			fd_;
	 unsigned int	registered_;
	 unsigned int	modes_;

 public:
	 User();
	 ~User();

	 // SETTERS
	 void	setNickName(const std::string &nickName);
	 void	setUserName(const std::string &userName);
	 void	setHostName(const std::string &hostName);
	 void	setServerName(const std::string &serverName);
	 void	setRealName(const std::string &realName);
	 void	setFd(int fd);
	 void	setRegistered(kExecCommand flag);
	 void	setMode(kUserMode mode);
	 void	unsetMode(kUserMode mode);
	 // GETTERS
	 const std::string&	getNickName() const;
	 const std::string&	getUserName() const;
	 const std::string&	getHostName() const;
	 const std::string&	getServerName() const;
	 const std::string&	getRealName() const;
	 int				getFd() const;
	 unsigned int		getRegistered() const;
	 unsigned int		getModes() const;
	 // DEBUG
	 void	printData() const;
};

#endif  // USER_HPP
