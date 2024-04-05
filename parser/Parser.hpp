#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

enum kTokenType {
	kCommand = 0,
	kParam
};

enum kCommandType {
	kString = 0,
	kDigit
};

enum kParamType {
	kNone = 0,
	kMiddle,
	kTrailing
};

class Token {
 private:
	 kTokenType	type_;
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
	// TODO(hnoguchi): Arrayを実装しても良い
	std::vector<Param>	params_;
 public:
	ParsedMessage();
	~ParsedMessage();

	// SETTER
	void	setCommand(const std::string &command);
	void	setParam(const std::string &param);
	// GETTER
	const std::string&			getCommand() const;
	const std::vector<Param>&	getParams() const;
	// debug
	void	printParsedMessage() const;
};

class Parser {
 private:
	std::string			message_;
	// TODO(hnoguchi): Token classは、一時的なものだから、parser();で生成する。よっていらない。
	std::vector<Token>	tokens_;
	ParsedMessage		parsed_;

 public:
	explicit Parser(const std::string &message);
	~Parser();

	// SETTER
	// TODO(hnoguchi): parser();で生成する。よっていらない。
	void	tokenize();
	void	parse();
	// GETTER
	// TODO(hnoguchi): parser();で生成する。よっていらない。
	const std::vector<Token>&	getTokens() const;
	const ParsedMessage&		getParsedMessage() const;
	// debug
	// TODO(hnoguchi): parser();で生成する。よっていらない。
	void	printTokens() const;
};

#endif  // PARSER_HPP
