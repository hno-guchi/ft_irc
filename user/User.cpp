#include "./User.hpp"

// CONSTRUCTORS and DESTRUCTORS
User::User() {}
User::~User() {}

// SETTERS
void	User::setNickName(const std::string &name) {
	this->nickName_ = name;
}

void	User::setHostName(const std::string &name) {
	this->hostName_ = name;
}

void	User::setUserName(const std::string &name) {
	this->userName_ = name;
}

void	User::setServerName(const std::string &name) {
	this->serverName_ = name;
}

// GETTERS
const std::string&	User::getNickName() const {
	return (this->nickName_);
}

const std::string&	User::getHostName() const {
	return (this->hostName_);
}

const std::string&	User::getUserName() const {
	return (this->userName_);
}

const std::string&	User::getServerName() const {
	return (this->serverName_);
}

