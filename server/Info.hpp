#ifndef INFO_HPP
# define INFO_HPP

#include <string>
#include <vector>
#include "./Config.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"

class Info : public Config {
 private:
	 std::vector<User*>		users_;
	 std::vector<Channel*>	channels_;

	 Info(const Info& rhs);
	 Info& operator=(const Info& rhs);

 public:
	 explicit Info(const std::string& connectPwd = "");
	 ~Info();
	 // GETTER
	 const std::vector<User*>&				getUsers() const;
	 const std::vector<Channel*>&			getChannels() const;
	 // SETTER

	 std::vector<User*>::iterator			findUser(const std::string& name);
	 std::vector<User*>::const_iterator		findUser(const std::string& name) const;
	 std::vector<User*>::iterator			findUser(int fd);
	 std::vector<User*>::const_iterator		findUser(int fd) const;
	 std::vector<Channel*>::iterator		findChannel(const std::string& name);
	 std::vector<Channel*>::const_iterator	findChannel(const std::string& name) const;
	 void									pushBackUser(User* user);
	 void									pushBackChannel(Channel* channel);
	 void									eraseUser(std::vector<User*>::iterator it);
	 void									eraseUserInChannel(User* user, Channel* channel);
	 void									eraseUserInChannels(User* user);
	 void									eraseChannel(std::vector<Channel*>::iterator it);

	 // debug
	 void									debugPrintUsers() const;
	 void									debugPrintChannels() const;
	 void									debugPrintInfo() const;
};

#endif
