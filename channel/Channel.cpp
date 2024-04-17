#include "./Channel.hpp"
#include "../user/User.hpp"

Channel::Channel(const std::string& name) :
	name_(name),  topic_(""), key_(""), limit_(-1), modes_(0) {}
Channel::~Channel() {}

// SETTER
void	Channel::setName(const std::string& name) {
	// TODO(hnoguchi): バリデーションは別でやる？
	if (name.size() == 0 || name.size() > 50) {
		return;
		// throw std::invalid_argument("Invalid name");
	}
	this->name_ = name;
}

void	Channel::setTopic(const std::string& topic) {
	// TODO(hnoguchi): バリデーションは別でやる？
	if (topic.size() == 0 || topic.size() > 100) {
		return;
		// throw std::invalid_argument("Invalid topin");
	}
	this->topic_ = topic;
}

void	Channel::setkey(const std::string& key) {
	// TODO(hnoguchi): バリデーションは別でやる？
	if (key.size() == 0 || key.size() > 30) {
		return;
		// throw std::invalid_argument("Invalid key");
	}
	// TODO(hnoguchi): バリデーションは別でやる？
	if ((this->modes_ & kKey) == 0) {
		return;
		// throw std::invalid_argument("Channel is not mode key protected.");
	}
	this->key_ = key;
}

void	Channel::setLimit(int limit) {
	// TODO(hnoguchi): バリデーションは別でやる？
	if (limit < 0 || limit > 5) {
		return;
		// throw std::invalid_argument("Invalid limit");
	}
	// TODO(hnoguchi): バリデーションは別でやる？
	if ((this->modes_ & kLimit) == 0) {
		return;
		// throw std::invalid_argument("Channel is not mode limit protected.");
	}
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

void	Channel::setMember(User* user) {
	// TODO(hnoguchi): バリデーションは別でやる？
	if (user == NULL) {
		// throw std::invalid_argument("Invalid argument");
		return;
	}
	// TODO(hnoguchi): バリデーションは別でやる？
	if ((this->modes_ & kLimit) && \
			static_cast<int>(this->members_.size()) >= this->limit_) {
		// throw std::invalid_argument("Channel is full");
		return;
	}
	try {
		this->members_.push_back(user);
	} catch (std::exception& e) {
		throw;
	}
}

void	Channel::setOperator(User* user) {
	try {
		this->operators_.push_back(user);
	} catch (std::exception& e) {
		throw;
	}
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
	try {
		this->members_.push_back(user);
	} catch (std::exception& e) {
		throw;
	}
}

void	Channel::addOperator(User* user) {
	try {
		this->operators_.push_back(user);
	} catch (std::exception& e) {
		throw;
	}
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
