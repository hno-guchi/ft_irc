#ifndef INFO_HPP
# define INFO_HPP

#include <vector>
#include "./Config.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"

class Info {
 private:
	 const Config			config_;
	 std::vector<User*>		users_;
	 std::vector<Channel*>	channels_;
 public:
	 Info();
	 ~Info();
	 // GETTER
	 const Config&							getConfig() const;
	 const std::vector<User*>&				getUsers() const;
	 const std::vector<Channel*>&			getChannels() const;
	 // SETTER
	 // void	setConfig(const Config& config);

	 std::vector<User*>::iterator			findUser(const std::string& name);
	 std::vector<User*>::const_iterator		findUser(const std::string& name) const;
	 std::vector<User*>::iterator			findUser(int fd);
	 std::vector<User*>::const_iterator		findUser(int fd) const;
	 std::vector<Channel*>::iterator			findChannel(const std::string& name);
	 std::vector<Channel*>::const_iterator	findChannel(const std::string& name) const;
	 void									pushBackUser(User* user);
	 void									pushBackChannel(Channel* channel);
	 void									eraseUser(std::vector<User*>::iterator it);
	 void									eraseChannel(std::vector<Channel*>::iterator it);

	 // debug
	 void									printConfig() const;
	 void									printUsers() const;
	 void									printChannels() const;
	 void									printInfo() const;
};

#endif
