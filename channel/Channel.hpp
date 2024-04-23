#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../user/User.hpp"

enum kChannelMode {
	kInviteOnly = (1 << 0),
	kKey = (1 << 1),
	kLimit = (1 << 2),
	// kChannelOperator = (1 << 3), // User classのkUserModeで扱う。
	kRestrictTopicSetting = (1 << 3)
};

class Channel {
 private:
	 // std::string		prefix_;
	 std::string		name_;
	 std::string		topic_;
	 std::string		key_;
	 int				limit_;
	 unsigned int		modes_;
	 // TODO(hnoguchi): ChannelUsers classにする？
	 std::vector<User*>	members_;
	 std::vector<User*>	operators_;

 public:
	 explicit Channel(const std::string& name);
	 ~Channel();

	 // SETTER
	 void						setName(const std::string& name);
	 void						setTopic(const std::string& topic);
	 void						setKey(const std::string& key);
	 void						setLimit(int limit);
	 void						setMode(kChannelMode mode);
	 void						unsetMode(kChannelMode mode);
	 void						setMember(User* user);
	 void						setOperator(User* user);
	 // GETTER
	const std::string&			getName() const;
	const std::string&			getTopic() const;
	const std::string&			getkey() const;
	int							getLimit() const;
	unsigned int				getModes() const;
	const std::vector<User*>&	getMembers() const;
	const std::vector<User*>&	getOperators() const;

	// TODO(hnoguchi): ChannelUsers classにする？
	void						addMember(User* user);
	void						addOperator(User* user);
	void						eraseMember(User* user);
	void						eraseOperator(User* oper);
};

#endif  // CHANNEL_HPP
