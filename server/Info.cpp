#include <vector>
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

const User&	Info::getUser(int i) const {
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
