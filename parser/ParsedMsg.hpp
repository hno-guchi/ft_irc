#ifndef PARSEDMSG_HPP
# define PARSEDMSG_HPP

#include <string>
#include <vector>
#include "./Token.hpp"
#include "./Param.hpp"

class ParsedMsg {
 private:
	std::string			command_;
	std::vector<Param>	params_;

 public:
	ParsedMsg();
	~ParsedMsg();

	// SETTER
	void						setCommand(const std::string& command);
	void						setParam(kTokenType tType, kParamType pType, const std::string& value);
	// GETTER
	const std::string&			getCommand() const;
	const std::vector<Param>&	getParams() const;
	// debug
	void						printParsedMsg() const;
};

#endif  // PARSEDMSG_HPP
