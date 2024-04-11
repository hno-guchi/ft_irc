#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

enum kTokenType {
	kCommand = 0,
	kMiddle,
	kTrailing
};

enum kCommandType {
	kString = 0,
	kDigit
};

enum kParamType {
	kNone = 0,
	kPMiddle,
	kPTrailing
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

class Param {
 private:
	kParamType	type_;
	std::string	value_;
 public:
	Param();
	~Param();

	// SETTER
	void	setType(const kParamType type);
	void	setValue(const std::string &value);
	// GETTER
	kParamType			getType() const;
	const std::string&	getValue() const;
	// debug
	void	printParam() const;
};

class ParsedMessage {
 private:
	kCommandType		type_;
	std::string			command_;
	std::vector<Param>	params_;
 public:
	ParsedMessage();
	~ParsedMessage();

	// SETTER
	void	setCommand(const std::string& command);
	void	setParam(const Token& token);
	// GETTER
	const std::string&			getCommand() const;
	const std::vector<Param>&	getParams() const;
	// debug
	void	printParsedMessage() const;
};

class Parser {
 private:
	ParsedMessage		parsed_;

 public:
	Parser();
	~Parser();

	// SETTER
	void						parse(std::string message);
	// GETTER
	const ParsedMessage&		getParsedMessage() const;
};

#endif  // PARSER_HPP
