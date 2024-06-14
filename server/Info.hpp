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

	 User*									findUser(const std::string& name);
	 const User*							findUser(const std::string& name) const;
	 User*									findUser(int fd);
	 const User*							findUser(int fd) const;
	 Channel*								findChannel(const std::string& name);
	 const Channel*							findChannel(const std::string& name) const;
	 void									pushBackUser(User* user);
	 void									pushBackChannel(Channel* channel);
	 void									eraseUser(User* user);
	 void									eraseChannel(Channel* channel);
	 void									eraseUserInChannel(User* user, Channel* channel);
	 void									eraseUserInChannels(User* user);

	 // debug
	 void									debugPrintUsers() const;
	 void									debugPrintChannels() const;
	 void									debugPrintInfo() const;
};

#endif
