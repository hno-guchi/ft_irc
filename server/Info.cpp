#include <vector>
#include <stdexcept>
#include "./Info.hpp"
#include "./Config.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"

Info::Info() : config_() {}
Info::~Info() {}

// GETTER
const Config&	Info::getConfig() const {
	return (this->config_);
}
const std::vector<User>&	Info::getUsers() const {
	return (this->users_);
}

const User&	Info::getUser(unsigned long i) const {
	if (this->users_.size() <= i) {
		throw std::out_of_range("Info::getUser()");
	}
	return (this->users_[i]);
}

const std::vector<Channel>&	Info::getChannels() const {
	return (this->channels_);
}

// SETTER
// void	Info::setConfig(const Config& config) {
// 	this->config_ = config;
// }

void	Info::addUser(const User& user) {
	try {
		this->users_.push_back(user);
	} catch (std::exception& e) {
		throw;
	}
}

void	Info::addChannel(const Channel& channel) {
	try {
		this->channels_.push_back(channel);
	} catch (std::exception& e) {
		throw;
	}
}

void	Info::eraseUser(User* user) {
	for (std::vector<User>::iterator it = this->users_.begin(); it != this->users_.end(); it++) {
		if (it->getNickName() == user->getNickName()) {
			this->users_.erase(it);
			return;
		}
	}
}

void	Info::eraseChannel(Channel* channel) {
	for (std::vector<Channel>::iterator it = this->channels_.begin(); it != this->channels_.end(); it++) {
		if (it->getName() == channel->getName()) {
			this->channels_.erase(it);
			return;
		}
	}
}
