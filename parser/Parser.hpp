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

	 std::string	toUpperString(const std::string& str);
	 std::string	toLowerString(const std::string& str);
	 void			tokenize(std::string *message, std::vector<Token> *tokens);
	 int			validTokens(const User& user, const std::vector<Token>& tokens, const std::string* cmdList);
	 int			validPing(const User& user, const std::vector<Token>& tokens);
	 int			validPass(const User& user, const std::vector<Token>& tokens);
	 int			validNick(const User& user, const std::vector<Token>& tokens);
	 int			validUser(const User& user, const std::vector<Token>& tokens);
	 int			validOper(const User& user, const std::vector<Token>& tokens);
	 int			validQuit(const User& user, const std::vector<Token>& tokens);
	 int			validJoin(const User& user, const std::vector<Token>& tokens);
	 int			validPart(const User& user, const std::vector<Token>& tokens);
	 int			validKick(const User& user, const std::vector<Token>& tokens);
	 int			validInvite(const User& user, const std::vector<Token>& tokens);
	 int			validTopic(const User& user, const std::vector<Token>& tokens);
	 int			validMode(const User& user, const std::vector<Token>& tokens);
	 int			validUserMode(const User& user, const std::vector<Token>& tokens);
	 int			validChannelMode(const User& user, const std::vector<Token>& tokens);
	 int			validPrivmsg(const User& user, const std::vector<Token>& tokens);
	 int			validNotice(const User& user, const std::vector<Token>& tokens);

	 Parser(const Parser &rhs);
	 Parser& operator=(const Parser &rhs);

 public:
	// CONSTRUCTOR & DESTRUCTOR
	Parser();
	~Parser();
	// SETTER
	int					parse(const User& user, std::string message, const std::string* cmdList);
	// GETTER
	const ParsedMsg&	getParsedMsg() const;
	bool				isCommand(const std::string& command, const std::string* cmdList);
	// DEBUG
	void				printTokens(const std::vector<Token>& tokens);
};

#endif  // PARSER_HPP
