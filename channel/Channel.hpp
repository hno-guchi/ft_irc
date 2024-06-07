#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../user/User.hpp"

enum kChannelMode {
	kInviteOnly = (1 << 0),
	kKeySet = (1 << 1),
	kLimit = (1 << 2),
	kRestrictTopicSetting = (1 << 3)
};

class Channel {
 private:
	 std::string		name_;
	 std::string		topic_;
	 std::string		key_;
	 int				limit_;
	 unsigned int		modes_;
	 std::vector<User*>	members_;
	 std::vector<User*>	invited_;
	 std::vector<User*>	operators_;

	 Channel(const Channel& src);
	 Channel& operator=(const Channel& src);

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

	 // GETTER
	 const std::string&			getName() const;
	 const std::string&			getTopic() const;
	 const std::string&			getKey() const;
	 int							getLimit() const;
	 unsigned int				getModes() const;
	 const std::vector<User*>&	getMembers() const;
	 const std::vector<User*>&	getInvited() const;
	 const std::vector<User*>&	getOperators() const;

	 void						pushBackMember(User* user);
	 void						pushBackInvited(User* user);
	 void						pushBackOperator(User* user);
	 void						eraseMember(User* user);
	 void						eraseInvited(User* user);
	 void						eraseOperator(User* user);
	 bool						isMember(User* user) const;
	 bool						isMember(const std::string& name) const;
	 bool						isInvited(const std::string& name) const;
	 bool						isInvited(User* user) const;
	 bool						isOperator(const std::string& name) const;
	 bool						isOperator(User* user) const;
	 void						resetData();
	 // debug
	 void						debugPrintChannel() const;
};

#endif  // CHANNEL_HPP
