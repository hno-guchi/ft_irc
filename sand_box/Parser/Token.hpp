#ifndef TOKEN_HPP
# define TOKEN_HPP

#include <string>

enum kTokenType {
	kCmdString = 0,
	kCmdDigit,
	kParamMiddle,
	kParamTrailing
};

class Token {
 private:
	 kTokenType		type_;
	 std::string	value_;
 public:
	 Token();
	 ~Token();

	// SETTER
	void	setType(const kTokenType type);
	void	setValue(const std::string &value);
	// GETTER
	kTokenType			getType() const;
	const std::string&	getValue() const;
	// debug
	void	printToken() const;
};


#endif  // TOKEN_HPP
