#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "./Token.hpp"
#include "./Param.hpp"
#include "./ParsedMsg.hpp"

class Parser {
 private:
	ParsedMsg	parsed_;

	void	tokenize(std::string *message, std::vector<Token> *tokens);
	int		validPing(std::vector<Token> *tokens);
	int		validPass(std::vector<Token> *tokens);
	int		validNick(std::vector<Token> *tokens);
	int		validUser(std::vector<Token> *tokens);
	int		validOper(std::vector<Token> *tokens);
	int		validQuit(std::vector<Token> *tokens);
	int		validJoin(std::vector<Token> *tokens);
	int		validPart(std::vector<Token> *tokens);
	int		validKick(std::vector<Token> *tokens);
	int		validInvite(std::vector<Token> *tokens);
	int		validTopic(std::vector<Token> *tokens);
	int		validMode(std::vector<Token> *tokens);
	int		validUserMode(std::vector<Token> *tokens);
	int		validChannelMode(std::vector<Token> *tokens);
	int		validPrivmsg(std::vector<Token> *tokens);
	int		validNotice(std::vector<Token> *tokens);
 public:
	Parser();
	~Parser();

	// SETTER
	int					parse(std::string message, const std::string* cmdList);
	// GETTER
	const ParsedMsg&	getParsedMsg() const;

	// void				tokenize(std::string *message, std::vector<Token> *tokens);
	bool				isCommand(const std::string& command, const std::string* cmdList);
	// DEBUG
	void				printTokens(const std::vector<Token>& tokens);
};

#endif  // PARSER_HPP
