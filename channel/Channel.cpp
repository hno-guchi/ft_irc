#include "./Channel.hpp"
#include "../user/User.hpp"

Channel::Channel(const std::string& name) :
	name_(name),  topic_(""), key_(""), limit_(-1), modes_(0) {}
Channel::~Channel() {}

// SETTER
void	Channel::setName(const std::string& name) {
	if (name.size() == 0 || name.size() > 50) {
		return;
		// throw std::invalid_argument("Invalid name");
	}
	this->name_ = name;
	return;
}

void	Channel::setTopic(const std::string& topic) {
	if (topic.size() == 0 || topic.size() > 100) {
		return;
		// throw std::invalid_argument("Invalid topin");
	}
	this->topic_ = topic;
	return;
}

void	Channel::setkey(const std::string& key) {
	if (key.size() == 0 || key.size() > 30) {
		return;
		// throw std::invalid_argument("Invalid key");
	}
	if ((this->modes_ & kKey) == 0) {
		return;
		// throw std::invalid_argument("Channel is not mode key protected.");
	}
	this->key_ = key;
	return;
}

void	Channel::setLimit(int limit) {
	if (limit < 0 || limit > 5) {
		return;
		// throw std::invalid_argument("Invalid limit");
	}
	if ((this->modes_ & kLimit) == 0) {
		return;
		// throw std::invalid_argument("Channel is not mode limit protected.");
	}
	this->limit_ = limit;
	return;
}

void	Channel::setMode(kChannelMode mode) {
	if (this->modes_ & mode) {
		return;
		// throw std::invalid_argument(Channel is already set to this mode.);
	}
	this->modes_ |= mode;
	return;
}

void	Channel::setMember(User* user) {
	if (user == NULL) {
		return;
		// throw std::invalid_argument("Invalid argument");
	}
	if ((this->modes_ & kLimit) && \
			static_cast<int>(this->members_.size()) >= this->limit_) {
		return;
		// throw std::invalid_argument("Channel is full");
	}
	this->members_.push_back(user);
	return;
}

void	Channel::setOperator(User* user) {
	this->operators_.push_back(user);
	return;
}

// GETTER
const std::string&	Channel::getName() const {
	return (this->name_);
}

const std::string&	Channel::getTopic() const {
	return (this->topic_);
}
const std::string&	Channel::getkey() const {
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

const std::vector<User*>&	Channel::getOperators() const {
	return (this->operators_);
}

void	Channel::addMember(User* user) {
	this->members_.push_back(user);
}

void	Channel::eraseMember(User* user) {
	for (std::vector<User *>::iterator it = this->members_.begin(); it != this->members_.end(); it++) {
		if (*it == user) {
			this->members_.erase(it);
			return;
		}
	}
}

void	Channel::eraseOperator(User* oper) {
	for (std::vector<User *>::iterator it = this->members_.begin(); it != this->members_.end(); it++) {
		if (*it == oper) {
			this->members_.erase(it);
			return;
		}
	}
}
