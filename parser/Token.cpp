#include <iostream>
#include "./Token.hpp"

Token::Token() {}
Token::~Token() {}

void	Token::setType(const kTokenType type) {
	this->type_ = type;
}

// SETTER
void	Token::setValue(const std::string &value) {
	this->value_ = value;
}

// GETTER
kTokenType	Token::getType() const {
	return (this->type_);
}

const std::string&	Token::getValue() const {
	return (this->value_);
}

// debug
void	Token::printToken() const {
	std::cout << "Type   : " << this->type_ << " | " << std::flush;
	std::cout << "Value  : [" << this->value_ << "]" << std::endl;
}
