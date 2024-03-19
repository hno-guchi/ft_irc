#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../user/User.hpp"

enum kChannelMode {
	kInviteOnly = (1 << 0),
	kTopicSetOnlyOperator = (1 << 1),
	kKey = (1 << 2),
	kLimit = (1 << 3)
};

class Channel {
 private:
	 std::string		name_;
	 std::string		topic_;
	 std::string		key_;
	 int				limit_;
	 unsigned int		modes_;
	 std::vector<User*>	members_;
	 std::vector<User*>	operators_;

 public:
	 explicit Channel(const std::string& name);
	 ~Channel();

	 // SETTER
	 void						setName(const std::string& name);
	 void						setTopic(const std::string& topic);
	 void						setkey(const std::string& key);
	 void						setLimit(int limit);
	 void						setModes(kChannelMode mode);
	 void						setMembers(User* user);
	 void						setOperators(User* user);
	 // GETTER
	const std::string&			getName() const;
	const std::string&			getTopic() const;
	const std::string&			getkey() const;
	int							getLimit() const;
	unsigned int				getModes() const;
	const std::vector<User*>	getMembers() const;
	const std::vector<User*>	getOperators() const;

	// ChannelManage class
	void						addMember(User* user);
};

#endif  // CHANNEL_HPP
