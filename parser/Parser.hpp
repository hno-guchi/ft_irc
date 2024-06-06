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
#include "../server/Info.hpp"
#include "../user/User.hpp"

class Parser {
 private:
	 ParsedMsg	parsed_;

	 std::string	toUpperString(const std::string& str);
	 std::string	toLowerString(const std::string& str);
	 void			tokenize(std::string *message, std::vector<Token> *tokens);
	 int			validRegisterTokens(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validTokens(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validPing(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validPass(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validRegisterNick(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validNick(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validUser(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validOper(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validQuit(const std::vector<Token>& tokens);
	 int			validJoin(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validPart(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validKick(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validInvite(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validTopic(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validMode(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validUserMode(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validChannelMode(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validPrivmsg(const User& user, const std::vector<Token>& tokens, const Info& info);
	 int			validNotice(const User& user, const std::vector<Token>& tokens, const Info& info);

	 Parser(const Parser &rhs);
	 Parser& operator=(const Parser &rhs);

 public:
	// CONSTRUCTOR & DESTRUCTOR
	Parser();
	~Parser();
	// SETTER
	int					registerParse(const User& user, std::string message, const Info& info);
	int					parse(const User& user, std::string message, const Info& info);
	// GETTER
	const ParsedMsg&	getParsedMsg() const;
	bool				isCommand(const std::string& command, const std::string* cmdList);
	// DEBUG
	void				printTokens(const std::vector<Token>& tokens);
};

#endif  // PARSER_HPP
