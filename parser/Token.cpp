#include <iostream>
#include "./Token.hpp"

// CONSTRUCTOR & DESTRUCTOR
Token::Token() {}

Token::Token(const Token &src) {
	this->operator=(src);
}

Token::~Token() {}

// OPERATOR
Token& Token::operator=(const Token &rhs) {
	if (this == &rhs) {
		return (*this);
	}
	this->type_ = rhs.getType();
	this->value_ = rhs.getValue();
	return (*this);
}

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
