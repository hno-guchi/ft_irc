#include "./Channel.hpp"
#include "../user/User.hpp"

Channel::Channel(const std::string& name) :
	name_(name),  topic_(""), key_(""), limit_(-1), modes_(0) {}
Channel::~Channel() {}

// SETTER
void	Channel::setName(const std::string& name) {
	// TODO(hnoguchi): バリデーションは別でやる？
	// if (name.size() == 0 || name.size() > 50) {
	// 	return;
	// 	// throw std::invalid_argument("Invalid name");
	// }
	this->name_ = name;
}

void	Channel::setTopic(const std::string& topic) {
	// TODO(hnoguchi): バリデーションは別でやる？
	// if (topic.size() == 0 || topic.size() > 100) {
	// 	return;
	// 	// throw std::invalid_argument("Invalid topin");
	// }
	this->topic_ = topic;
}

void	Channel::setKey(const std::string& key) {
	// TODO(hnoguchi): バリデーションは別でやる？
	// if (key.size() == 0 || key.size() > 30) {
	// 	return;
	// 	// throw std::invalid_argument("Invalid key");
	// }
	// TODO(hnoguchi): バリデーションは別でやる？
	// if (!(this->modes_ & kKey)) {
	// 	return;
	// 	// throw std::invalid_argument("Channel is not mode key protected.");
	// }
	this->key_ = key;
}

void	Channel::setLimit(int limit) {
	// TODO(hnoguchi): バリデーションは別でやる？
	// if (limit < 0 || limit > 5) {
	// 	return;
	// 	// throw std::invalid_argument("Invalid limit");
	// }
	// TODO(hnoguchi): バリデーションは別でやる？
	// if ((this->modes_ & kLimit) == 0) {
	// 	return;
	// 	// throw std::invalid_argument("Channel is not mode limit protected.");
	// }
	this->limit_ = limit;
}

void	Channel::setMode(kChannelMode mode) {
	// TODO(hnoguchi): バリデーションは別でやる？
	if (this->modes_ & mode) {
		return;
		// throw std::invalid_argument(Channel is already set to this mode.);
	}
	this->modes_ |= mode;
}

void	Channel::unsetMode(kChannelMode mode) {
	this->modes_ ^= (this->modes_ & mode);
}

// GETTER
const std::string&	Channel::getName() const {
	return (this->name_);
}

const std::string&	Channel::getTopic() const {
	return (this->topic_);
}
const std::string&	Channel::getKey() const {
	return (this->key_);
}

int	Channel::getLimit() const {
	return (this->limit_);
}

unsigned int	Channel::getModes() const {
	return (this->modes_);
}

const std::vector<User*>&	Channel::getMembers() const {
	return (this->members_);
}

const std::vector<User*>&	Channel::getInvited() const {
	return (this->invited_);
}

const std::vector<User*>&	Channel::getOperators() const {
	return (this->operators_);
}

void	Channel::pushBackMember(User* user) {
	try {
		this->members_.push_back(user);
	} catch (std::exception& e) {
		throw;
	}
}

void	Channel::pushBackInvited(User* user) {
	try {
		this->invited_.push_back(user);
	} catch (std::exception& e) {
		throw;
	}
}

void	Channel::pushBackOperator(User* user) {
	try {
		this->operators_.push_back(user);
	} catch (std::exception& e) {
		throw;
	}
}

void	Channel::eraseMember	(User* user) {
	for (std::vector<User*>::iterator it = this->members_.begin(); it != this->members_.end(); it++) {
		if (*it == user) {
			this->members_.erase(it);
			return;
		}
	}
}

void	Channel::eraseInvited	(User* user) {
	for (std::vector<User*>::iterator it = this->invited_.begin(); it != this->invited_.end(); it++) {
		if (*it == user) {
			this->invited_.erase(it);
			return;
		}
	}
}

void	Channel::eraseOperator	(User* user) {
	for (std::vector<User*>::iterator it = this->operators_.begin(); it != this->operators_.end(); it++) {
		if (*it == user) {
			this->operators_.erase(it);
			return;
		}
	}
}

void	Channel::resetDate() {
	try {
	 	this->name_ = "";
	 	this->topic_ = "";
	 	this->key_ = "";
	 	this->limit_ = -1;
	 	this->modes_ = 0;
		for (std::vector<User*>::iterator it = this->members_.begin(); it != this->members_.end(); it++) {
			this->members_.erase(it);
		}
		for (std::vector<User*>::iterator it = this->invited_.begin(); it != this->invited_.end(); it++) {
			this->invited_.erase(it);
		}
		for (std::vector<User*>::iterator it = this->operators_.begin(); it != this->operators_.end(); it++) {
			this->operators_.erase(it);
		}
	} catch (std::exception& e) {
		throw;
	}
}

bool	Channel::isMember(const std::string& name) const {
	for (std::vector<User*>::const_iterator it = this->getMembers().begin(); it != this->getMembers().end(); it++) {
		if ((*it)->getNickName() == name) {
			return (true);
		}
	}
	return (false);
}

bool	Channel::isInvited(const std::string& name) const {
	for (std::vector<User*>::const_iterator it = this->getInvited().begin(); it != this->getInvited().end(); it++) {
		if ((*it)->getNickName() == name) {
			return (true);
		}
	}
	return (false);
}

bool	Channel::isOperator(const std::string& name) const {
	for (std::vector<User*>::const_iterator it = this->getOperators().begin(); it != this->getOperators().end(); it++) {
		if ((*it)->getNickName() == name) {
			return (true);
		}
	}
	return (false);
}

void	Channel::printData() const {
	std::cout << "[" << this->getName() << "]" << "__________" << std::endl;
	std::cout << "[topic]     | [" << this->getName() << "]" << std::endl;
	std::cout << "[key]       | [" << this->getKey() << "]" << std::endl;
	std::cout << "[limit]     | [" << this->getLimit() << "]" << std::endl;
	std::cout << "[modes]     | [" << this->getModes() << "]" << std::endl;
	std::cout << "[operators]" << std::endl;
	for (std::vector<User*>::const_iterator it = this->getOperators().begin(); it != this->getOperators().end(); it++) {
		std::cout << "            | [" << (*it)->getNickName() << "]" << std::endl;
	}
	std::cout << "[members]" << std::endl;
	for (std::vector<User*>::const_iterator it = this->getMembers().begin(); it != this->getMembers().end(); it++) {
		std::cout << "            | [" << (*it)->getNickName() << "]" << std::endl;
	}
	std::cout << "[invited]" << std::endl;
	for (std::vector<User*>::const_iterator it = this->getInvited().begin(); it != this->getInvited().end(); it++) {
		std::cout << "            | [" << (*it)->getNickName() << "]" << std::endl;
	}
	std::cout << "______________________________________" << std::endl;
}
