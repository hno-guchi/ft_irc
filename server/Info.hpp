#ifndef INFO_HPP
# define INFO_HPP

#include <vector>
#include "./Config.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"

class Info {
 private:
	 const Config			config_;
	 // TODO(hnoguchi): 定数で配列のサイズを指定する。
	 std::vector<User>		users_;
	 // TODO(hnoguchi): std::mapの方が良い？
	 std::vector<Channel>	channels_;
 public:
	 Info();
	 ~Info();
	 // GETTER
	 const Config&					getConfig() const;
	 const std::vector<User>&		getUsers() const;
	 const User&					getUser(int i) const;
	 const std::vector<Channel>&	getChannels() const;
	 // const Channel&	getChannel(const std::string& name) const;
	 // SETTER
	 // void	setConfig(const Config& config);
	 void	addUser(const User& user);
	 void	addChannel(const Channel& channel);
};

#endif
