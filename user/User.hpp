#ifndef USER_HPP
# define USER_HPP

#include <string>

class User {
 private:
	 std::string	nickName_;
	 std::string	hostName_;
	 std::string	userName_;
	 std::string	serverName_;
 public:
	 User();
	 ~User();

	 // SETTERS
	 void	setNickName(const std::string &nickName);
	 void	setHostName(const std::string &hostName);
	 void	setUserName(const std::string &userName);
	 void	setServerName(const std::string &serverName);
	 // GETTERS
	 const std::string&	getNickName() const;
	 const std::string&	getHostName() const;
	 const std::string&	getUserName() const;
	 const std::string&	getServerName() const;
};

#endif  // USER_HPP
