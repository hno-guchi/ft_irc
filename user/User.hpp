#ifndef USER_HPP
# define USER_HPP

#include <iostream>
#include <string>
#include "../color.hpp"

class User {
 private:
	 std::string	nickName_;
	 std::string	hostName_;
	 std::string	userName_;
	 std::string	serverName_;
	 int			fd_;

 public:
	 User();
	 ~User();

	 // SETTERS
	 void	setNickName(const std::string &nickName);
	 void	setHostName(const std::string &hostName);
	 void	setUserName(const std::string &userName);
	 void	setServerName(const std::string &serverName);
	 void	setFd(int fd);
	 // GETTERS
	 const std::string&	getNickName() const;
	 const std::string&	getHostName() const;
	 const std::string&	getUserName() const;
	 const std::string&	getServerName() const;
	 int				getFd() const;
	 // DEBUG
	 void	printData() const;
};

#endif  // USER_HPP
