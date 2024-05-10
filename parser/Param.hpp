#ifndef PARAM_HPP
# define PARAM_HPP

#include <string>
#include "./Token.hpp"

enum kParamType {
	kNone = 0,
	kServer,
	kPassword,
	kNickName,
	kUserName,
	kHostName,
	kServerName,
	kRealName,
	kMessage,
	kMode,
	kChannel,
	kModeParam,
	kKey,
	kTopic,
	kMsgTarget,
	kText
};

class Param {
 private:
	kTokenType	tType_;
	kParamType	pType_;
	std::string	value_;
 public:
	Param();
	~Param();

	// SETTER
	void				setTokenType(const kTokenType type);
	void				setParamType(const kParamType type);
	void				setValue(const std::string &value);
	// GETTER
	kTokenType			getTokenType() const;
	kParamType			getParamType() const;
	const std::string&	getValue() const;
	// debug
	void				printParam() const;
};

# endif  // PARAM_HPP
