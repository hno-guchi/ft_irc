#include <iostream>
#include "./Param.hpp"
#include "./Token.hpp"

Param::Param() {}

Param::Param(const Param& src) {
	this->operator=(src);
}

Param::~Param() {}

Param& Param::operator=(const Param& rhs) {
	if (this == &rhs) {
		return (*this);
	}
	this->tType_ = rhs.getTokenType();
	this->pType_ = rhs.getParamType();
	this->value_ = rhs.getValue();
	return (*this);
}

void	Param::setTokenType(const kTokenType type) {
	this->tType_ = type;
}

void	Param::setParamType(const kParamType type) {
	this->pType_ = type;
}

// SETTER
void	Param::setValue(const std::string &value) {
	this->value_ = value;
}

// GETTER
kTokenType	Param::getTokenType() const {
	return (this->tType_);
}

kParamType	Param::getParamType() const {
	return (this->pType_);
}

const std::string&	Param::getValue() const {
	return (this->value_);
}

// debug
void	Param::printParam() const {
	std::cout << "tType   : " << this->tType_ << " | " << std::flush;
	std::cout << "pType   : " << this->pType_ << " | " << std::flush;
	std::cout << "Value  : [" << this->value_ << "]" << std::endl;
}
