#include "./Parser.hpp"
#include "./ParsedMsg.hpp"
#include "./Token.hpp"
#include "./Param.hpp"
#include "./ValidParam.hpp"
#include "../../color.hpp"
// #include "../../error/error.hpp"

static std::string	toUpperString(const std::string& str) {
	std::string	ret = str;
	std::locale	l = std::locale::classic();

	for (std::string::iterator it = ret.begin(); it != ret.end(); it++) {
		*it = std::toupper(*it, l);
	}
	return (ret);
}

static std::string	toLowerString(const std::string& str) {
	std::string	ret = str;
	std::locale	l = std::locale::classic();

	for (std::string::iterator it = ret.begin(); it != ret.end(); it++) {
		*it = std::tolower(*it, l);
	}
	return (ret);
}

Parser::Parser() {}
Parser::~Parser() {}

void	Parser::tokenize(std::string *message, std::vector<Token> *tokens) {
	try {
		if (message->empty()) {
			return;
			// throw std::invalid_argument("Message is empty.");
		}
		std::string			word("");
		std::istringstream	msgStream(*message);
		std::cin.clear(std::ios::goodbit);
		while (word.empty()) {
			std::getline(msgStream, word, ' ');
		}
		if (std::cin.fail()) {
			throw std::invalid_argument("Failed to read message.");
		}
		Token	command;
		command.setType(kCmdString);
		command.setValue(word);
		tokens->push_back(command);
		std::cin.clear(std::ios::goodbit);
		while (std::getline(msgStream, word, ' ')) {
			if (word.empty()) {
				continue;
			}
			Token	param;
			if (word[0] != ':') {
				param.setType(kParamMiddle);
			} else {
				word = word.substr(1);
				std::string	trailing("");
				std::cin.clear(std::ios::goodbit);

				std::getline(msgStream, trailing);
				if (std::cin.fail()) {
					throw std::invalid_argument("Failed to read message.");
				}
				if (trailing.size() > 0) {
					word += " " + trailing;
				}
				param.setType(kParamTrailing);
			}
			param.setValue(word);
			tokens->push_back(param);
		}
		if (std::cin.fail()) {
			throw std::invalid_argument("Failed to read message.");
		}
	} catch (std::exception& e) {
		throw;
	}
}

int	Parser::parse(std::string message, const std::string* cmdList) {
// int	Parser::parse(std::string message, const Info& info) {
	ValidParam	validParam;
	if (message.empty()) {
		// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
		return (-1);
	}
	// check message length
	try {
		// TODO(hnoguchi): TokenList classを作成した方が良い？
		std::vector<Token>	tokens;

		this->tokenize(&message, &tokens);
		if (tokens.empty()) {
			// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
			return (-1);
		}
		if (!validParam.isCommand(tokens[0].getValue(), cmdList)) {
			// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
			return (-1);
		}
		this->parsed_.setCommand(toUpperString(tokens[0].getValue()));
		// command毎にパラメータのバリデーションを行う。
		if (this->parsed_.getCommand() == "PING") {
			// PING <server1> [ <server2> ]
			if (tokens.size() < 2 || tokens.size() > 3) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			if (!validParam.isHostName(tokens[1].getValue())) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			this->parsed_.setParam(kServer, tokens[1].getValue());
			if (tokens.size() == 3) {
				if (!validParam.isHostName(tokens[2].getValue())) {
					// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
					return (-1);
				}
				this->parsed_.setParam(kServer, tokens[2].getValue());
			}
		} else if (this->parsed_.getCommand() == "PASS") {
			// PASS <password>
			if (tokens.size() != 2) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			if (!validParam.isPassword(tokens[1].getValue())) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			this->parsed_.setParam(kPassword, tokens[1].getValue());
		} else if (this->parsed_.getCommand() == "NICK") {
			// NICK <nickname>
			if (tokens.size() != 2) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			if (!validParam.isNickName(tokens[1].getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
		} else if (this->parsed_.getCommand() == "USER") {
			// USER <username> <hostname> <servername> <realname>
			if (tokens.size() != 5) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			if (!validParam.isUser(tokens[1].getValue())) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			this->parsed_.setParam(kUserName, tokens[1].getValue());
			if (!validParam.isHostName(tokens[2].getValue())) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			this->parsed_.setParam(kHostName, tokens[2].getValue());
			if (!validParam.isHostName(tokens[3].getValue())) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			this->parsed_.setParam(kHostName, tokens[3].getValue());
			this->parsed_.setParam(kRealName, tokens[4].getValue());
		} else if (this->parsed_.getCommand() == "OPER") {
			// OPER <name> <password>
			if (tokens.size() != 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!validParam.isNickName(tokens[1].getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
			this->parsed_.setParam(kNickName, tokens[1].getValue());
			if (!validParam.isPassword(tokens[2].getValue())) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			this->parsed_.setParam(kPassword, tokens[2].getValue());
		} else if (this->parsed_.getCommand() == "QUIT") {
			// QUIT [ <Quit Message> ]
			if (tokens.size() == 2) {
				if (validParam.isMessage(tokens[1].getValue())) {
					this->parsed_.setParam(kMessage, tokens[1].getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "JOIN") {
			// JOIN <channel> [ <key> ]
			if (tokens.size() < 2 || tokens.size() > 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!validParam.isChannel(tokens[1].getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1].getValue()));
			if (tokens.size() == 3) {
				if (validParam.isKey(tokens[2].getValue())) {
					this->parsed_.setParam(kKey, tokens[2].getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "PART") {
			// PART <channel> [ <Part Message> ]
			if (tokens.size() < 2 || tokens.size() > 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!validParam.isChannel(tokens[1].getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1].getValue()));
			if (tokens.size() == 3) {
				if (validParam.isMessage(tokens[2].getValue())) {
					this->parsed_.setParam(kMessage, tokens[2].getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "KICK") {
			// KICK <channel> <user> [<comment>]
			if (tokens.size() != 3 && tokens.size() != 4) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!validParam.isChannel(tokens[1].getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1].getValue()));
			if (!validParam.isNickName(tokens[2].getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
			this->parsed_.setParam(kNickName, tokens[2].getValue());
			if (tokens.size() == 4) {
				if (validParam.isMessage(tokens[3].getValue())) {
					this->parsed_.setParam(kMessage, tokens[3].getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "INVITE") {
			// INVITE <nickname> <channel>
			if (tokens.size() != 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!validParam.isNickName(tokens[1].getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
			this->parsed_.setParam(kNickName, tokens[1].getValue());
			if (!validParam.isChannel(tokens[2].getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[2].getValue()));
		} else if (this->parsed_.getCommand() == "TOPIC") {
			// TOPIC <channel> [ <topic> ]
			if (tokens.size() != 2 && tokens.size() != 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!validParam.isChannel(tokens[1].getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1].getValue()));
			if (tokens.size() == 3) {
				if (validParam.isTopic(tokens[2].getValue())) {
					this->parsed_.setParam(kTopic, tokens[2].getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "MODE") {
			// MODE <nickname> [ ( "+" / "-" ) ( "o" ) ]
			if (tokens.size() < 2 || tokens.size() > 4) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (tokens[1].getValue()[0] != '#') {
				if (!validParam.isNickName(tokens[1].getValue())) {
					// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
					return (-1);
				}
				this->parsed_.setParam(kNickName, tokens[1].getValue());
				if (tokens.size() == 3) {
					if (tokens[2].getValue().size() != 2) {
						return (-1);
					}
					if (tokens[2].getValue()[0] != '+' && tokens[2].getValue()[0] != '-') {
						return (-1);
					}
					if (tokens[2].getValue()[1] != 'o') {
						return (-1);
					}
				}
				this->parsed_.setParam(kMode, tokens[2].getValue());
			} else if (!validParam.isChannel(tokens[1].getValue())) {
			// MODE <channel>  [ ( "-" / "+" ) ( "i" / "k" / "l" / "o" / "t" ) [ <modeparam> ] ]
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1].getValue()));
			if (tokens.size() > 2) {
				if (tokens[2].getValue().size() != 2) {
					return (-1);
				}
				if (tokens[2].getValue()[0] != '+' && tokens[2].getValue()[0] != '-') {
					return (-1);
				}
				// unsigned long	pos = info->getConfig().getChannelModes().find(parsedMsg.getParams()[1].getValue()[1]);
				// if (pos == std::string::npos) {
				// 	return (Reply::errUnknownMode(kERR_UNKNOWNMODE,user->getNickName(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue()));
				// }
				if (tokens[2].getValue()[1] != 'i' && tokens[2].getValue()[1] != 'k' && tokens[2].getValue()[1] != 'l' && tokens[2].getValue()[1] != 'o' && tokens[2].getValue()[1] != 't') {
					return (-1);
				}
				this->parsed_.setParam(kMode, tokens[2].getValue());
			}
			// Check <modeparam>
			if (tokens.size() == 4) {
				// Invite	<modeparam> == <nickname>
				if (tokens[2].getValue()[1] == 'i') {
					if (!validParam.isNickName(tokens[3].getValue())) {
						// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
						return (-1);
					}
				// Key		<modeparam> == <key>
				} else if (tokens[2].getValue()[1] == 'k') {
					if (!validParam.isKey(tokens[3].getValue())) {
						return (-1);
					}
				// Limit	<modeparam> == digit(< 5)
				} else if (tokens[2].getValue()[1] == 'l') {
					if (tokens[3].getValue().size() > 1) {
						return (-1);
					}
					if (!std::isdigit(tokens[3].getValue()[0])) {
						return (-1);
					}
				// Operator	<modeparam> == <nickname>
				} else if (tokens[2].getValue()[1] == 'o') {
					if (!validParam.isNickName(tokens[3].getValue())) {
						// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
						return (-1);
					}
				// Topic	<modeparam> == <topic>
				} else if (tokens[2].getValue()[1] == 't') {
					if (!validParam.isTopic(tokens[3].getValue())) {
						return (-1);
					}
				}
			}
			this->parsed_.setParam(kModeParam, tokens[3].getValue());
		// PRIVMSG <msgtarget> <text>
		} else if (this->parsed_.getCommand() == "PRIVMSG") {
			if (tokens.size() != 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (tokens[1].getValue()[0] != '#') {
				if (!validParam.isNickName(tokens[1].getValue())) {
					// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
					return (-1);
				}
			} else if (!validParam.isChannel(tokens[1].getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kMsgTarget, tokens[1].getValue());
			if (!validParam.isTrailing(tokens[2].getValue())) {
				return (-1);
			}
			if (tokens[2].getValue().size() > 400) {
				// 412 ERR_NOMOTD ":No message of the day is available"
				return (-1);
			}
			this->parsed_.setParam(kText, tokens[2].getValue());
		// NOTICE <msgtarget> <text>
		} else if (this->parsed_.getCommand() == "NOTICE") {
			if (tokens.size() != 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (tokens[1].getValue()[0] != '#') {
				if (!validParam.isNickName(tokens[1].getValue())) {
					// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
					return (-1);
				}
			} else if (!validParam.isChannel(tokens[1].getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kMsgTarget, tokens[1].getValue());
			if (!validParam.isTrailing(tokens[2].getValue())) {
				return (-1);
			}
			if (tokens[2].getValue().size() > 400) {
				// 412 ERR_NOMOTD ":No message of the day is available"
				return (-1);
			}
			this->parsed_.setParam(kText, tokens[2].getValue());
		// } else if (this->parsed_.getCommand() == "PONG") {
		// } else if (this->parsed_.getCommand() == "ERROR") {
		}
		return (0);
	} catch (std::exception& e) {
		std::cerr << RED << e.what() << END << std::endl;
		// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
		return (-1);
		// throw;
	}
}

const ParsedMsg&	Parser::getParsedMsg() const {
	return (this->parsed_);
}

void	Parser::printTokens(const std::vector<Token>& tokens) {
	// std::cout << YELLOW << "[Tokens]    ____________________" << END << std::endl;
	// std::cout << "[Type] : [Value]" << std::endl;
	for (std::vector<Token>::const_iterator it = tokens.begin(); it != tokens.end(); it++) {
		std::cout << "  " << it->getType() << ": [" << it->getValue() << "]" << std::endl;
	}
	std::cout << YELLOW << "_________________________________" << END << std::endl;
}
