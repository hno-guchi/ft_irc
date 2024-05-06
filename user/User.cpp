#include <unistd.h>
#include "./User.hpp"
#include "../error/error.hpp"

// CONSTRUCTORS and DESTRUCTORS
User::User() : index_(-1), fd_(NULL), registered_(0), modes_(0) {
}

User::~User() {
	if (this->fd_ != NULL) {
		if (*(this->fd_) != -1) {
			close(*(this->fd_));
			*(this->fd_) = -1;
		}
		this->fd_ = NULL;
	}
}

// OPERATOR
// User&	User::operator=(const User& rhs) {
// 	if (this == &rhs) {
// 		return (*this);
// 	}
// 	try {
// 		this->nickName_ = rhs.getNickName();
// 		this->userName_ = rhs.getUserName();
// 		this->hostName_ = rhs.getHostName();
// 		this->serverName_ = rhs.getServerName();
// 		this->realName_ = rhs.getRealName();
// 		this->replyName_ = rhs.getReplyName();
// 		this->index_ = rhs.getIndex();
// 		this->fd_ = rhs.getFdAddr();
// 		this->registered_ = rhs.getRegistered();
// 		this->modes_ = rhs.getModes();
// 		return (*this);
// 	} catch (std::exception &e) {
// 		fatalError(e.what());
// 		throw;
// 	}
// }


// SETTERS
void	User::setNickName(const std::string &name) {
	this->nickName_ = name;
}

void	User::setUserName(const std::string &name) {
	this->userName_ = name;
}

void	User::setHostName(const std::string &name) {
	this->hostName_ = name;
}

void	User::setServerName(const std::string &name) {
	this->serverName_ = name;
}

void	User::setRealName(const std::string &name) {
	this->realName_ = name;
}

void	User::setReplyName(const std::string &name) {
	this->replyName_ = name;
}

void	User::setIndex(int i) {
	this->index_ = i;
}

void	User::setFd(int* fd) {
	this->fd_ = fd;
}

void	User::setRegistered(kExecCommand cmd) {
	this->registered_ |= cmd;
}

void	User::setMode(kUserMode mode) {
	this->modes_ |= mode;
}

void	User::unsetMode(kUserMode mode) {
	this->modes_ ^= (this->modes_ & mode);
}

// GETTERS
const std::string&	User::getNickName() const {
	return (this->nickName_);
}

const std::string&	User::getUserName() const {
	return (this->userName_);
}

const std::string&	User::getHostName() const {
	return (this->hostName_);
}

const std::string&	User::getServerName() const {
	return (this->serverName_);
}

const std::string&	User::getRealName() const {
	return (this->realName_);
}

const std::string&	User::getReplyName() const {
	return (this->replyName_);
}

int	User::getIndex() const {
	return (this->index_);
}

// int*	User::getFdAddr() const {
// 	return (this->fd_);
// }

int	User::getFd() const {
	return (*(this->fd_));
}

unsigned int	User::getRegistered() const {
	return (this->registered_);
}

unsigned int	User::getModes() const {
	return (this->modes_);
}

void	User::disconnect() {
	try {
		if (this->fd_ == NULL) {
			return;
		}
		if (*(this->fd_) == -1) {
			return;
		}
		if (close(*(this->fd_)) == -1) {
			throw std::runtime_error("close");
		}
		*(this->fd_) = -1;
		this->fd_ = NULL;
	} catch (std::exception &e) {
		fatalError(e.what());
		throw;
	}
}

void	User::resetDate() {
	try {
		this->nickName_ = "";
		this->userName_ = "";
		this->hostName_ = "";
		this->serverName_ = "";
		this->realName_ = "";
		this->replyName_ = "";
		this->index_ = -1;
		this->registered_ = 0;
		this->modes_ = 0;
	} catch (std::exception &e) {
		fatalError(e.what());
		throw;
	}
}

// DEBUG
void	User::printData() const {
	std::cout << MAGENTA << "[USER INFO] --------------------" << END << std::endl;
	std::cout << "[index]        : [" << this->index_ << "]" << std::endl;
	std::cout << "[fd]           : [(" << this->fd_ << ")" << std::flush;
	if (this->fd_ != NULL) {
		std::cout << *(this->fd_) << "]" << std::endl;
	} else {
		std::cout << "NULL]" << std::endl;
	}
	std::cout << "[modes]        : [" << this->modes_ << "]" << std::endl;
	std::cout << "[registered]   : [" << this->registered_ << "]" << std::endl;
	std::cout << "[nickName]     : [" << this->nickName_ << "]" << std::endl;
	std::cout << "[hostName]     : [" << this->hostName_ << "]" << std::endl;
	std::cout << "[userName]     : [" << this->userName_ << "]" << std::endl;
	std::cout << "[serverName]   : [" << this->serverName_ << "]" << std::endl;
	std::cout << "[realName]     : [" << this->realName_ << "]" << std::endl;
	std::cout << "[replyName]     : [" << this->replyName_ << "]" << std::endl;
	std::cout << MAGENTA << "--------------------------------\n" << END << std::endl;
}
