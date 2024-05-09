#ifndef PARAM_HPP
# define PARAM_HPP

#include <string>

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
	kParamType	type_;
	std::string	value_;
 public:
	Param();
	~Param();

	// SETTER
	void				setType(const kParamType type);
	void				setValue(const std::string &value);
	// GETTER
	kParamType			getType() const;
	const std::string&	getValue() const;
	// debug
	void				printParam() const;
};

# endif  // PARAM_HPP
