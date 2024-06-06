#ifndef USER_HPP
# define USER_HPP

#include <poll.h>
#include <iostream>
#include <string>
#include "../color.hpp"

enum kUserMode {
	kOperator = (1 << 0)
};

enum kExecCommand {
	kPassCommand = (1 << 0),
	kNickCommand = (1 << 1),
	kUserCommand = (1 << 2),
	kExecAllCmd = (kPassCommand | kNickCommand | kUserCommand)
};

class User {
 private:
	 std::string	nickName_;
	 std::string	userName_;
	 std::string	hostName_;
	 std::string	serverName_;
	 std::string	realName_;
	 int			index_;
	 int*			fd_;
	 unsigned int	registered_;
	 unsigned int	modes_;
	 std::string	leftMsg_;

	 explicit User(const User& rhs);
	 User&	operator=(const User& rhs);

 public:
	 explicit User(int* fd = NULL);
	 ~User();

	 // SETTERS
	 void	setNickName(const std::string &name);
	 void	setUserName(const std::string &name);
	 void	setHostName(const std::string &name);
	 void	setServerName(const std::string &name);
	 void	setRealName(const std::string &name);
	 void	setIndex(int i);
	 void	setFd(int* fd);
	 void	setRegistered(kExecCommand flag);
	 void	setMode(kUserMode mode);
	 void	setLeftMsg(const std::string &msg);
	 void	unsetMode(kUserMode mode);
	 // GETTERS
	 const std::string&	getNickName() const;
	 const std::string&	getUserName() const;
	 const std::string&	getHostName() const;
	 const std::string&	getServerName() const;
	 const std::string&	getRealName() const;
	 const std::string	getPrefixName() const;
	 int				getIndex() const;
	 // int*				getFdAddr() const;
	 int				getFd() const;
	 unsigned int		getRegistered() const;
	 unsigned int		getModes() const;
	 const std::string&	getLeftMsg() const;

	 void				disconnect();
	 void				resetData();
	 // DEBUG
	 void	debugPrintUser() const;
};

#endif  // USER_HPP
