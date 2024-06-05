#include "./Parser.hpp"
#include "./ParsedMsg.hpp"
#include "./Token.hpp"
#include "./Param.hpp"
#include "./ValidParam.hpp"
#include "../color.hpp"
#include "../server/Info.hpp"
#include "../reply/Reply.hpp"
#include "../debug/debug.hpp"

Parser::Parser() {}
Parser::~Parser() {}

std::string	Parser::toUpperString(const std::string& str) {
	std::string	ret = str;
	std::locale	l = std::locale::classic();

	for (std::string::iterator it = ret.begin(); it != ret.end(); it++) {
		*it = std::toupper(*it, l);
	}
	return (ret);
}

std::string	Parser::toLowerString(const std::string& str) {
	std::string	ret = str;
	std::locale	l = std::locale::classic();

	for (std::string::iterator it = ret.begin(); it != ret.end(); it++) {
		*it = std::tolower(*it, l);
	}
	return (ret);
}

void	Parser::tokenize(std::string *message, std::vector<Token> *tokens) {
	try {
		if (message->empty()) {
			return;
		}
		std::string				word("");
		std::string::size_type	startPos(0);

		// TODO(hnoguchi): std::string::substr(); throw exception std::out_of_range();
		while (startPos < message->size() && message->at(startPos) == ' ') {
			startPos++;
		}
		if (startPos == message->size()) {
			return;
		}
		std::string::size_type	delimPos = message->find(' ', startPos);
		if (delimPos == message->npos) {
			word = message->substr(startPos);
			startPos = message->size();
		} else {
			word = message->substr(startPos, delimPos - startPos);
			startPos = delimPos + 1;
		}
		Token	command;
		command.setType(kCmdString);
		command.setValue(word);
		tokens->push_back(command);
		while(startPos < message->size()) {
			Token	param;
			delimPos = message->find(' ', startPos);
			if (delimPos == message->npos && startPos < message->size()) {
				word = message->substr(startPos);
				startPos = message->size();
			} else {
				word = message->substr(startPos, delimPos - startPos);
				startPos = delimPos;
			}
			if (!word.empty()) {
				if (word[0] != ':') {
					param.setType(kParamMiddle);
				} else {
					param.setType(kParamTrailing);
					// word = word.substr(1);
					if (startPos < message->size()) {
						word += message->substr(startPos);
						startPos = message->size();
					}
				}
				param.setValue(word);
				tokens->push_back(param);
			}
			startPos += 1;
		}
	} catch (std::exception& e) {
		throw;
	}
}

int	Parser::validTokens(const User& user, const std::vector<Token>& tokens, const std::string* cmdList) {
	try {
		ValidParam	validParam;

		for (std::vector<Token>::const_iterator it = tokens.begin(); it != tokens.end(); it++) {
			if (it->getType() == kCmdString) {
				if (!validParam.isCommand(it->getValue(), cmdList)) {
					std::string reply = reply::errUnknownCommand(kERR_UNKNOWNCOMMAND, user.getPrefixName(), it->getValue());
					Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
					return (-1);
				}
			} else if (it->getType() == kParamMiddle) {
				if (!validParam.isMiddle(it->getValue())) {
					// std::string reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), it->getValue());
					// Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
					return (-1);
				}
			} else if (it->getType() == kParamTrailing) {
				if (!validParam.isTrailing(it->getValue())) {
					// std::string reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), it->getValue());
					// Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
					return (-1);
				}
			}
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// PING <server1> [ <server2> ]
int	Parser::validPing(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() < 2 || tokens.size() > 3) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "PING");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kServer, tokens[1].getValue());
		if (tokens.size() == 3) {
			this->parsed_.setParam(tokens[2].getType(), kServer, tokens[2].getValue());
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// PASS <password>
int	Parser::validPass(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() != 2) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "PASS");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kPassword, tokens[1].getValue());
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// NICK <nickname>
int	Parser::validNick(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() != 2) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "NICK");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kNickName, tokens[1].getValue());
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// USER <username> <hostname> <servername> <realname>
int	Parser::validUser(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() != 5) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "USER");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kUserName, tokens[1].getValue());
		this->parsed_.setParam(tokens[2].getType(), kHostName, tokens[2].getValue());
		this->parsed_.setParam(tokens[3].getType(), kHostName, tokens[3].getValue());
		this->parsed_.setParam(tokens[4].getType(), kRealName, tokens[4].getValue());
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// OPER <name> <password>
int	Parser::validOper(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens->size() != 3) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "OPER");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kNickName, tokens[1].getValue());
		this->parsed_.setParam(tokens[2].getType(), kPassword, tokens[2].getValue());
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// QUIT [ <Quit Message> ]
int	Parser::validQuit(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() == 2) {
			this->parsed_.setParam(tokens[1].getType(), kMessage, tokens[1].getValue());
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// JOIN <channel> [ <key> ]
int	Parser::validJoin(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() < 2 || tokens.size() > 3) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "JOIN");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kChannel, this->toLowerString(tokens[1].getValue()));
		if (tokens->size() == 3) {
			this->parsed_.setParam(tokens[2].getType(), kKey, tokens[2].getValue());
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// PART <channel> [ <Part Message> ]
int	Parser::validPart(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() < 2 || tokens.size() > 3) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "PART");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kChannel, this->toLowerString(tokens[1].getValue()));
		if (tokens->size() == 3) {
			this->parsed_.setParam(tokens[2].getType(), kMessage, tokens[2].getValue());
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// KICK <channel> <user> [<comment>]
int	Parser::validKick(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() != 3 && tokens.size() != 4) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "KICK");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kChannel, this->toLowerString(tokens[1].getValue()));
		this->parsed_.setParam(tokens[2].getType(), kNickName, tokens[2].getValue());
		if (tokens.size() == 4) {
			this->parsed_.setParam(tokens[3].getType(), kMessage, tokens[3].getValue());
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// INVITE <nickname> <channel>
int	Parser::validInvite(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() != 3) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "INVITE");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kNickName, tokens[1].getValue());
		this->parsed_.setParam(tokens[2].getType(), kChannel, this->toLowerString(tokens[2].getValue()));
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// TOPIC <channel> [ <topic> ]
int	Parser::validTopic(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() != 2 && tokens.size() != 3) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "TOPIC");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kChannel, this->toLowerString(tokens[1].getValue()));
		if (tokens->size() == 3) {
			this->parsed_.setParam(tokens[2].getType(), kTopic, tokens[2].getValue());
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// MODE <nickname> [ ( "+" ) ( "r" ) ]
int	Parser::validUserMode(const User& user, const std::vector<Token>& tokens) {
	try {
		this->parsed_.setParam(tokens[1].getType(), kNickName, tokens[1].getValue());
		if (tokens.size() == 3) {
			if (tokens[2].getValue().size() != 2) {
				std::string reply = Reply::errUModeUnknownFlag(kERR_UMODEUNKNOWNFLAG, user.getPrefixName());
				Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
				return (-1);
			}
			this->parsed_.setParam(tokens[2].getType(), kMode, tokens[2].getValue());
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// MODE <channel>  [ ( "-" / "+" ) ( "i" / "k" / "l" / "o" / "t" ) [ <modeparam> ] ]
int	Parser::validChannelMode(const User& user, const std::vector<Token>& tokens) {
	try {
		this->parsed_.setParam(tokens[1].getType(), kChannel, this->toLowerString(tokens[1].getValue()));
		if (tokens.size() > 2) {
			if (tokens[2].getValue().size() != 2) {
				std::string	reply = reply::errUnknownMode(kERR_UNKNOWNMODE, user.getPrefixName(), tokens[2].getValue(), tokens[1].getValue());
				Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
				return (-1);
			}
			if (tokens[2].getValue()[0] != '+' && tokens[2].getValue()[0] != '-') {
				std::string	reply = reply::errUnknownMode(kERR_UNKNOWNMODE, user.getPrefixName(), tokens[2].getValue(), tokens[1].getValue());
				Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
				return (-1);
			}
			this->parsed_.setParam(tokens[2].getType(), kMode, tokens[2].getValue());
		}
		// Check <modeparam>
		if (tokens.size() == 4) {
			this->parsed_.setParam(tokens[3].getType(), kModeParam, tokens[3].getValue());
		}
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

int	Parser::validMode(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() < 2 || tokens.size() > 4) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "MODE");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		if (tokens[1].getValue()[0] != '#') {
			return (this->validUserMode(tokens));
		}
		return (this->validChannelMode(tokens));
	} catch (std::exception& e) {
		throw;
	}
}

// PRIVMSG <msgtarget> <text>
int	Parser::validPrivmsg(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens.size() != 3) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "PRIVMSG");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam(tokens[1].getType(), kMsgTarget, tokens[1].getValue());
		if (tokens[2].getValue().size() > 400) {
			std::string	reply = reply::errNoTextToSend(kERR_NOTEXTTOSEND, user.getPrefixName(), "PRIVMSG");
			// 412 ERR_NOMOTD ":No message of the day is available"
			std::cerr << RED << "Too long text." << END << std::endl;
			return (-1);
		}
		this->parsed_.setParam((*tokens)[2].getType(), kText, (*tokens)[2].getValue());
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

// NOTICE <msgtarget> <text>
int	Parser::validNotice(const User& user, const std::vector<Token>& tokens) {
	try {
		if (tokens->size() != 3) {
			std::string	reply = reply::errNeedMoreParams(kERR_NEEDMOREPARAMS, user.getPrefixName(), "NOTICE");
			Server::sendNonBlocking(user.getFd(), reply.c_str(), reply.size());
			return (-1);
		}
		this->parsed_.setParam((*tokens)[1].getType(), kMsgTarget, (*tokens)[1].getValue());
		if ((*tokens)[2].getValue().size() > 400) {
			// 412 ERR_NOMOTD ":No message of the day is available"
			std::cerr << RED << "Too long text." << END << std::endl;
			return (-1);
		}
		this->parsed_.setParam((*tokens)[2].getType(), kText, (*tokens)[2].getValue());
		return (0);
	} catch (std::exception& e) {
		throw;
	}
}

int	Parser::parse(const User& user, std::string message, const std::string* cmdList) {
	if (message.empty()) {
		return (-1);
	}
	if (message.size() > 510) {
		return (-1);
	}
	try {
		std::vector<Token>	tokens;

		this->tokenize(&message, &tokens);
		if (tokens.empty()) {
			return (-1);
		}
		if (this->validTokens(user, tokens, cmdList) == -1) {
			return (-1);
		}
		this->parsed_.setCommand(this->toUpperString(tokens[0].getValue()));
		if (this->parsed_.getCommand() == "PING") {
			return (this->validPing(user, tokens));
		} else if (this->parsed_.getCommand() == "PASS") {
			return (this->validPass(&tokens));
		} else if (this->parsed_.getCommand() == "NICK") {
			return (this->validNick(&tokens));
		} else if (this->parsed_.getCommand() == "USER") {
			return (this->validUser(&tokens));
		} else if (this->parsed_.getCommand() == "OPER") {
			return (this->validOper(&tokens));
		} else if (this->parsed_.getCommand() == "QUIT") {
			return (this->validQuit(&tokens));
		} else if (this->parsed_.getCommand() == "JOIN") {
			return (this->validJoin(&tokens));
		} else if (this->parsed_.getCommand() == "PART") {
			return (this->validPart(&tokens));
		} else if (this->parsed_.getCommand() == "KICK") {
			return (this->validKick(&tokens));
		} else if (this->parsed_.getCommand() == "INVITE") {
			return (this->validInvite(&tokens));
		} else if (this->parsed_.getCommand() == "TOPIC") {
			return (this->validTopic(&tokens));
		} else if (this->parsed_.getCommand() == "MODE") {
			return (this->validMode(&tokens));
		} else if (this->parsed_.getCommand() == "PRIVMSG") {
			return (this->validPrivmsg(&tokens));
		} else if (this->parsed_.getCommand() == "NOTICE") {
			return (this->validNotice(&tokens));
		}
		return (0);
	} catch (std::exception& e) {
		debugPrintErrorMessage(e.what());
		throw;
	}
}

const ParsedMsg&	Parser::getParsedMsg() const {
	return (this->parsed_);
}

// DEBUG
void	Parser::printTokens(const std::vector<Token>& tokens) {
	// std::cout << YELLOW << "[Tokens]    ____________________" << END << std::endl;
	// std::cout << "[Type] : [Value]" << std::endl;
	for (std::vector<Token>::const_iterator it = tokens.begin(); it != tokens.end(); it++) {
		std::cout << "  " << it->getType() << ": [" << it->getValue() << "]" << std::endl;
	}
	std::cout << YELLOW << "_________________________________" << END << std::endl;
}
