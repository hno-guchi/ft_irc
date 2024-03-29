#include "./User.hpp"

// CONSTRUCTORS and DESTRUCTORS
User::User() :
	fd_(-1) {}

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

void	User::setFd(int fd) {
	this->fd_ = fd;
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

int	User::getFd() const {
	return (this->fd_);
}

// DEBUG
void	User::printData() const {
	std::cout << MAGENTA << "[USER INFO] --------------------" << END << std::endl;
	std::cout << "[fd]         : [" << this->fd_ << "]" << std::endl;
	std::cout << "[nickName]   : [" << this->nickName_ << "]" << std::endl;
	std::cout << "[hostName]   : [" << this->hostName_ << "]" << std::endl;
	std::cout << "[userName]   : [" << this->userName_ << "]" << std::endl;
	std::cout << "[serverName] : [" << this->serverName_ << "]" << std::endl;
	std::cout << MAGENTA << "--------------------------------\n" << END << std::endl;
}
