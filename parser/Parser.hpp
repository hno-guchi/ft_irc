#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

enum kTokenType {
	kUnknown = 0,
	kCommand,
	kParam
};

enum kCommandType {
	kNotType = 0,
	kString,
	kDigit
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

class Command {
	private:
		kCommandType				type_;
		std::string					command_;
		std::vector<std::string>	params_;
	public:
		Command();
		~Command();

		// SETTER
		void	setCommand(const std::string &command);
		void	setParams(const std::string &param);

		// GETTER
		const std::string&				getCommand() const;
		const std::vector<std::string>&	getParams() const;
		// debug
		void	printCommand() const;
};

class Parser {
	private:
		std::string				message_;
		// std::vector<std::string>	messages_;
		std::vector<Token>		tokens_;
		std::vector<Command>	commands_;
	public:
		explicit Parser(const std::string &message);
		~Parser();

		// SETTER
		void	tokenize();
		void	parse();
		// GETTER
		const std::vector<Token>&	getTokens() const;
		// debug
		void	printTokens() const;
};

#endif  // PARSER_HPP
