#ifndef VALIDPARAM_HPP
# define VALIDPARAM_HPP

# include <string>
# include "./IsChar.hpp"

class ValidParam : private IsChar {
 public:
	 bool	isMiddle(const std::string &middle);
	 bool	isTrailing(const std::string &trailing);

	 bool	isCommand(const std::string& command, const std::string* cmdList);

	 bool	isPassword(const std::string& password);
	 bool	isNickName(const std::string& nickName);
	 bool	isUser(const std::string& user);
	 bool	isShortName(const std::string& shortName);
	 bool	isHostName(const std::string& hostName);
	 bool	isChannel(const std::string& channel);
	 bool	isMessage(const std::string& message);
	 bool	isTopic(const std::string& topic);
	 bool	isKey(const std::string& key);
};

#endif  // VALIDPARAM_HPP
