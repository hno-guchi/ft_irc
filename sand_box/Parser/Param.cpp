#include <iostream>
#include "./Param.hpp"

Param::Param() {}
Param::~Param() {}

void	Param::setType(const kParamType type) {
	this->type_ = type;
}

// SETTER
void	Param::setValue(const std::string &value) {
	this->value_ = value;
}

// GETTER
kParamType	Param::getType() const {
	return (this->type_);
}

const std::string&	Param::getValue() const {
	return (this->value_);
}

// debug
void	Param::printParam() const {
	std::cout << "Type   : " << this->type_ << " | " << std::flush;
	std::cout << "Value  : [" << this->value_ << "]" << std::endl;
}
