#include <vector>
#include <stdexcept>
#include "./Info.hpp"
#include "./Config.hpp"
#include "../debug/debug.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"

Info::Info(const std::string& connectPwd) : Config(connectPwd) {}
Info::~Info() {}

// GETTER
const std::vector<User*>&	Info::getUsers() const {
	return (this->users_);
}

const std::vector<Channel*>&	Info::getChannels() const {
	return (this->channels_);
}

std::vector<User*>::iterator	Info::findUser(const std::string& name) {
	for (std::vector<User*>::iterator it = this->users_.begin(); it != this->users_.end(); it++) {
		if ((*it)->getNickName() != name) {
			continue;
		}
		return (it);
	}
	return (this->users_.end());
}

std::vector<User*>::const_iterator	Info::findUser(const std::string& name) const {
	for (std::vector<User*>::const_iterator it = this->users_.begin(); it != this->users_.end(); it++) {
		if ((*it)->getNickName() != name) {
			continue;
		}
		return (it);
	}
	return (this->users_.end());
}

std::vector<User*>::iterator	Info::findUser(int fd) {
	for (std::vector<User*>::iterator it = this->users_.begin(); it != this->users_.end(); it++) {
		if ((*it)->getFd() != fd) {
			continue;
		}
		return (it);
	}
	return (this->users_.end());
}

std::vector<User*>::const_iterator	Info::findUser(int fd) const {
	for (std::vector<User*>::const_iterator it = this->users_.begin(); it != this->users_.end(); it++) {
		if ((*it)->getFd() != fd) {
			continue;
		}
		return (it);
	}
	return (this->users_.end());
}

std::vector<Channel*>::iterator	Info::findChannel(const std::string& name) {
	for (std::vector<Channel*>::iterator it = this->channels_.begin(); it != this->channels_.end(); it++) {
		if ((*it)->getName() != name) {
			continue;
		}
		return (it);
	}
	return (this->channels_.end());
}

std::vector<Channel*>::const_iterator	Info::findChannel(const std::string& name) const {
	for (std::vector<Channel*>::const_iterator it = this->channels_.begin(); it != this->channels_.end(); it++) {
		if ((*it)->getName() != name) {
			continue;
		}
		return (it);
	}
	return (this->channels_.end());
}

// SETTER
// void	Info::setConfig(const Config& config) {
// 	this->config_ = config;
// }

void	Info::pushBackUser(User* user) {
	try {
		this->users_.push_back(user);
	} catch (std::exception& e) {
		debugPrintErrorMessage(e.what());
		throw std::invalid_argument("Info::pushBackUser()");
	}
}

void	Info::pushBackChannel(Channel* channel) {
	try {
		this->channels_.push_back(channel);
	} catch (std::exception& e) {
		debugPrintErrorMessage(e.what());
		throw std::invalid_argument("Info::pushBackChannel()");
	}
}

void	Info::eraseUser(std::vector<User*>::iterator it) {
	try {
		(*it)->disconnect();
		(*it)->resetData();
		delete *it;
		this->users_.erase(it);
	} catch (std::exception& e) {
		debugPrintErrorMessage(e.what());
		throw std::invalid_argument("Info::deleteUser()");
	}
}

void	Info::eraseChannel(std::vector<Channel*>::iterator it) {
	try {
		(*it)->resetData();
		delete *it;
		this->channels_.erase(it);
	} catch (std::exception& e) {
		debugPrintErrorMessage(e.what());
		throw std::invalid_argument("Info::deleteChannel()");
	}
}

void	Info::debugPrintUsers() const {
	std::cout << "[users_]    Current size (" << this->users_.size() << ")" << std::endl;
	for (std::vector<User*>::const_iterator it = this->users_.begin(); it != this->users_.end(); it++) {
		(*it)->debugPrintUser();
		std::cout << std::endl;
	}
}

void	Info::debugPrintChannels() const {
	std::cout << "[channels_] Current size (" << this->channels_.size() << ")" << std::endl;
	for (std::vector<Channel*>::const_iterator it = this->channels_.begin(); it != this->channels_.end(); it++) {
		(*it)->debugPrintChannel();
		std::cout << std::endl;
	}
}

void	Info::debugPrintInfo() const {
	this->debugPrintConfig();
	this->debugPrintUsers();
	this->debugPrintChannels();
}
