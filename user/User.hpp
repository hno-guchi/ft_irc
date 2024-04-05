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

class User {
 private:
	 std::string	nickName_;
	 std::string	hostName_;
	 std::string	userName_;
	 std::string	serverName_;
	 bool			isRegistered_;
	 int			fd_;
	 unsigned int	modes_;

 public:
	 User();
	 ~User();

	 // SETTERS
	 void	setNickName(const std::string &nickName);
	 void	setHostName(const std::string &hostName);
	 void	setUserName(const std::string &userName);
	 void	setServerName(const std::string &serverName);
	 void	setIsRegistered(bool flag);
	 void	setFd(int fd);
	 void	setMode(kUserMode mode);
	 void	unsetMode(kUserMode mode);
	 // GETTERS
	 const std::string&	getNickName() const;
	 const std::string&	getHostName() const;
	 const std::string&	getUserName() const;
	 const std::string&	getServerName() const;
	 bool				getIsRegistered() const;
	 int				getFd() const;
	 unsigned int		getModes() const;
	 // DEBUG
	 void	printData() const;
};

#endif  // USER_HPP
