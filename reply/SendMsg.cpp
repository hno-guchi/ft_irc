#include "./SendMsg.hpp"

SendMsg::SendMsg(int *fd) : fd_(fd) {}

SendMsg::~SendMsg() {}

// SETTER
void	SendMsg::setPrefix(const std::string& prefix) {
	this->prefix_ = prefix;
}
void	SendMsg::setMsg(const std::string& msg) {
	this->msg_ = msg;
}

// GETTER
const std::string&	SendMsg::getPrefix() const {
	return (this->prefix_);
}

const std::string&	SendMsg::getMsg() const {
	return (this->msg_);
}
