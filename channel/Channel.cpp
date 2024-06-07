#include <algorithm>
#include "./Channel.hpp"
#include "../color.hpp"
#include "../user/User.hpp"

Channel::Channel(const std::string& name) :
	name_(name),  topic_(""), key_(""), limit_(-1), modes_(0) {}
Channel::~Channel() {}

// SETTER
void	Channel::setName(const std::string& name) {
	this->name_ = name;
}

void	Channel::setTopic(const std::string& topic) {
	this->topic_ = topic;
}

void	Channel::setKey(const std::string& key) {
	this->key_ = key;
}

void	Channel::setLimit(int limit) {
	this->limit_ = limit;
}

void	Channel::setMode(kChannelMode mode) {
	if (this->modes_ & mode) {
		return;
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

void	Channel::eraseMember(User* user) {
	if (std::find(this->members_.begin(), this->members_.end(), user) == this->members_.end()) {
		return;
	}
	this->members_.erase(std::find(this->members_.begin(), this->members_.end(), user));
}

void	Channel::eraseInvited(User* user) {
	if (std::find(this->invited_.begin(), this->invited_.end(), user) == this->invited_.end()) {
		return;
	}
	this->invited_.erase(std::find(this->invited_.begin(), this->invited_.end(), user));
}

void	Channel::eraseOperator(User* user) {
	if (std::find(this->operators_.begin(), this->operators_.end(), user) == this->operators_.end()) {
		return;
	}
	this->operators_.erase(std::find(this->operators_.begin(), this->operators_.end(), user));
}

void	Channel::resetData() {
	try {
		this->name_ = "";
		this->topic_ = "";
		this->key_ = "";
		this->limit_ = -1;
		this->modes_ = 0;
		this->members_.clear();
		this->invited_.clear();
		this->operators_.clear();
	} catch (std::exception& e) {
		throw;
	}
}

bool	Channel::isMember(User* user) const {
	if (std::find(this->members_.begin(), this->members_.end(), user) == this->members_.end()) {
		return (false);
	}
	return (true);
}

bool	Channel::isMember(const std::string& name) const {
	for (std::vector<User*>::const_iterator it = this->getMembers().begin(); it != this->getMembers().end(); it++) {
		if ((*it)->getNickName() == name) {
			return (true);
		}
	}
	return (false);
}

bool	Channel::isInvited(User* user) const {
	if (std::find(this->invited_.begin(), this->invited_.end(), user) == this->invited_.end()) {
		return (false);
	}
	return (true);
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
	for (std::vector<User*>::const_iterator it = this->operators_.begin(); it != this->operators_.end(); it++) {
		if ((*it)->getNickName() == name) {
			return (true);
		}
	}
	return (false);
}

bool	Channel::isOperator(User* user) const {
	if (std::find(this->operators_.begin(), this->operators_.end(), user) == this->operators_.end()) {
		return (false);
	}
	return (true);
}

void	Channel::debugPrintChannel() const {
	std::cout << CYAN << "[CHANNEL INFO] -----------------" << END << std::endl;
	std::cout << "[name]      | [" << this->getName() << "]" << std::endl;
	std::cout << "[topic]     | [" << this->getName() << "]" << std::endl;
	std::cout << "[key]       | [" << this->getKey() << "]" << std::endl;
	std::cout << "[limit]     | [" << this->getLimit() << "]" << std::endl;
	std::cout << "[modes]     | [" << this->getModes() << "]" << std::endl;
	std::cout << "[operators] | " << std::flush;
	for (std::vector<User*>::const_iterator it = this->getOperators().begin(); it != this->getOperators().end(); it++) {
		if (it != this->getOperators().begin()) {
			std::cout << "            | " << std::flush;
		}
		std::cout << "[" << (*it)->getNickName() << "]" << std::flush;
		if ((it + 1) != this->getOperators().end()) {
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
	std::cout << "[members]   | " << std::flush;
	for (std::vector<User*>::const_iterator it = this->getMembers().begin(); it != this->getMembers().end(); it++) {
		if (it != this->getMembers().begin()) {
			std::cout << "            | " << std::flush;
		}
		std::cout << "[" << (*it)->getNickName() << "]" << std::flush;
		if ((it + 1) != this->getMembers().end()) {
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
	std::cout << "[invited]   | " << std::flush;
	for (std::vector<User*>::const_iterator it = this->getInvited().begin(); it != this->getInvited().end(); it++) {
		if (it != this->getInvited().begin()) {
			std::cout << "            | " << std::flush;
		}
		std::cout << "[" << (*it)->getNickName() << "]" << std::flush;
		if ((it + 1) != this->getInvited().end()) {
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
	std::cout << CYAN << "--------------------------------\n" << END << std::endl;
}
