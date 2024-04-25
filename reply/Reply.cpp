#include "./Reply.hpp"
#include "../parser/Parser.hpp"
#include "../error/error.hpp"

// CONSTRUCTOR
// DESTRUCTOR
Reply::Reply() : delimiter_("\r\n") {}
Reply::~Reply() {}

std::string	Reply::createPrefix(const std::string& prefix) {
	try {
		std::string	message = ":" + prefix + " ";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplWelcome(const std::string& toName, const std::string& userName, const std::string& serverName) {
	try {
		std::string	message = "001 " + toName + " :Welcome to the Internet Relay Network " + toName + "!" + userName + "@" + serverName + "\r\n";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplYourHost(const std::string& toName, const std::string& serverName, const std::string& version) {
	try {
		std::string	message = "002 " + toName + " :Your host is " + serverName + ", running version " + version + "\r\n";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplCreated(const std::string& toName, const time_t& createdDate) {
	try {
		std::string	date = std::asctime(std::localtime(&(createdDate)));
		if (!date.empty() && date[date.size() - 1] == '\n') {
			date.erase(date.size() - 1);
		}
		std::string	message = "003 " + toName + " :This server was created " + date + "\r\n";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplMyInfo(const std::string& toName, const Config& config) {
	try {
		std::string	message = "004 " + toName + " :" + config.getServerName() + " " + config.getVersion() + " " + config.getUserModes() + " " +  config.getChannelModes() + "\r\n";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplUModeIs(const User& user) {
	try {
		std::string	message = "221 " + user.getNickName();
		if (user.getModes() & kAway) {
			message += " +a";
		} else if (user.getModes() & kOperator) {
			message += " +o";
		} else if (user.getModes() & kRestrict) {
			message += " +r";
		}
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplChannelModeIs(const std::string& channel, const std::string& mode, const std::string& param) {
	try {
		std::string	message = "324 " + channel + " " + mode;
		if (!param.empty()) {
			message += " " + param;
		}
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplNoTopic(const std::string& channel) {
	try {
		std::string	message = "331 " + channel + " :No topic is set";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplTopic(const std::string& channel, const std::string& topic) {
	try {
		std::string	message = "332 " + channel + " :" + topic;
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplInviting(const std::string& channel, const std::string& nickName) {
	try {
		std::string	message = "341 " + channel + " " + nickName;
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::rplYourOper(const std::string& nickName) {
	try {
		std::string	message = "381 " + nickName + " :You are now an IRC operator";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::errPrefix(kErrReplyNum num, const std::string& prefix) {
	try {
		std::stringstream	ss;
		ss << static_cast<int>(num);
		std::string	message = ss.str() + " " + prefix + " :";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::errNoSuchNick(const std::string& nickName) {
	try {
		std::string	message = nickName + " :No such nick/channel\r\n";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::errNoSuchServer(const std::string& serverName) {
	try {
		std::string	message = serverName + " :No such server";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::errNoSuchChannel(const std::string& channelName) {
	try {
		std::string	message = channelName + " :No such channel";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::errNoOrigin() {
	try {
		std::string	message = "No origin specified";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::errNoRecipient(const std::string& command) {
	try {
		std::string	message = "No recipient given (" + command + ")";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::errNoTextToSend() {
	try {
		std::string	message = "No text to send";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::errUnKnownCommand(const std::string& command) {
	try {
		std::string	message = command + " :Unknown command";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}
std::string	Reply::errNoNickNameGiven() {
	try {
		std::string	message = "No nickname given";
		return (message);
	} catch (const std::exception& e) {
		fatalError(e.what());
		return ("");
	}
}

std::string	Reply::createMessage(int num, const User& user, const Info& info, const ParsedMessage& parsedMsg) {
	if (num <= 0) {
		return ("");
	}
	try {
		std::string	msg = this->createPrefix(info.getConfig().getServerName());
		if (num == kRPL_WELCOME) {
			msg += this->rplWelcome(user.getNickName(), user.getUserName(), user.getServerName());
			msg += this->createPrefix(info.getConfig().getServerName());
			msg += this->rplYourHost(user.getNickName(), info.getConfig().getServerName(), info.getConfig().getVersion());
			msg += this->createPrefix(info.getConfig().getServerName());
			msg += this->rplCreated(user.getNickName(), info.getConfig().getCreatedData());
			msg += this->createPrefix(info.getConfig().getServerName());
			msg += this->rplMyInfo(user.getNickName(), info.getConfig());
			msg += ":" + user.getNickName() + " NICK :" + user.getNickName();
		} else if (num < 100  || (num >= 200 && num < 400)) {
			if (num == kRPL_UMODEIS) {
				msg += this->rplUModeIs(user);
			} else if (num == kRPL_CHANNELMODEIS) {
				if (parsedMsg.getParams().size() > 2) {
					msg += this->rplChannelModeIs(parsedMsg.getParams()[0].getValue(), parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[2].getValue());
				} else {
					msg += this->rplChannelModeIs(parsedMsg.getParams()[0].getValue(), parsedMsg.getParams()[1].getValue(), "");
				}
			} else if (num == kRPL_NOTOPIC) {
				// TOPIC
				msg += rplNoTopic(parsedMsg.getParams()[0].getValue());
			} else if (num == kRPL_TOPIC) {
				// JOIN, TOPIC
				msg += rplTopic(parsedMsg.getParams()[0].getValue(), parsedMsg.getParams()[1].getValue());
			} else if (num == kRPL_INVITING) {
				// INVITE
				msg += rplInviting(parsedMsg.getParams()[1].getValue(), parsedMsg.getParams()[0].getValue());
			} else if (num == kRPL_YOUREOPER) {
				// OPER
				msg += rplYourOper(user.getNickName());
			}
		} else if (num >= 400 && num < 600) {
			msg += this->errPrefix(static_cast<kErrReplyNum>(num), user.getNickName());
			if (num == kERR_NOSUCHNICK) {
				// PRIVMSG, INVITE
				// msg += this->errPrefix(kERR_NOSUCHNICK, user.getNickName());
				msg += this->errNoSuchNick(parsedMsg.getParams()[0].getValue());
			} else if (num == kERR_NOSUCHSERVER) {
				// msg += this->errPrefix(kERR_NOSUCHSERVER, user.getNickName());
				msg += this->errNoSuchServer(parsedMsg.getParams()[0].getValue());
			} else if (num == kERR_NOSUCHCHANNEL) {
				// JOIN, KICK, PART
				// msg += this->errPrefix(kERR_NOSUCHCHANNEL, user.getNickName());
				msg += this->errNoSuchChannel(parsedMsg.getParams()[0].getValue());
			} else if (num == kERR_NOORIGIN) {
				// PONG
				// msg += this->errPrefix(kERR_NOORIGIN, user.getNickName());
				msg += this->errNoOrigin();
			} else if (num == kERR_NORECIPIENT) {
				// NOTICE, PRIVMSG
				// msg += this->errPrefix(kERR_NORECIPIENT, user.getNickName());
				msg += this->errNoRecipient(parsedMsg.getCommand());
			} else if (num == kERR_NOTEXTTOSEND) {
				// NOTICE, PRIVMSG
				// msg += this->errPrefix(kERR_NOTEXTTOSEND, user.getNickName());
				msg += this->errNoTextToSend();
			} else if (num == kERR_UNKNOWNCOMMAND) {
				// msg += this->errPrefix(kERR_UNKNOWNCOMMAND, user.getNickName());
				msg += this->errUnKnownCommand(parsedMsg.getCommand());
			} else if (num == kERR_NONICKNAMEGIVEN) {
				// msg += this->errPrefix(kERR_NONICKNAMEGIVEN, user.getNickName());
				msg += errNoNickNameGiven();
			} else if (num == kERR_ERRONEUSNICKNAME) {
				msg += "432 " + user.getNickName() + " :" + parsedMsg.getParams()[0].getValue() + " :Erroneous nickname";
			} else if (num == kERR_NICKNAMEINUSE) {
				// msg += "433 " + user.getNickName() + " :" + parsedMsg.getParams()[0].getValue() + " :Nickname is already in use";
				msg += "433 " + user.getNickName() + " :" + parsedMsg.getParams()[0].getValue();
			} else if (num == kERR_USERNOTINCHANNEL) {
				msg += "441 " + user.getNickName() + " " + parsedMsg.getParams()[0].getValue() + " :They aren't on that channel";
			} else if (num == kERR_NOTONCHANNEL) {
				msg += "442 " + user.getNickName() + " :" + parsedMsg.getParams()[0].getValue() + " :You're not on that channel";
				// TODO(hnoguchi): INVITEコマンドの場合、parsedMsg.getParams()[1].getValue()がchannel名
				// msg += "442 " + user.getNickName() + " :" + parsedMsg.getParams()[0].getValue() + " :You're not on that channel";
			} else if (num == kERR_USERONCHANNEL) {
				// INVITE
				msg += "443 " + user.getNickName() + " " + parsedMsg.getParams()[1].getValue() + " :is already on channel";
			} else if (num == kERR_NOTREGISTERED) {
				msg += "451 * :You have not registered";
			} else if (num == kERR_NEEDMOREPARAMS) {
				msg += "461 " + user.getNickName() + " :" + parsedMsg.getCommand() + " :Not enough parameters";
			} else if (num == kERR_ALREADYREGISTRED) {
				msg += "462 " +  user.getNickName() + " :Unauthorized command (already registered)";
			} else if (num == kERR_PASSWDMISMATCH) {
				msg += "464 " +  user.getNickName() + " :Password incorrect";
			} else if (num == kERR_KEYSET) {
				msg += "467 " +  user.getNickName() + " " + parsedMsg.getParams()[0].getValue() + " :Channel key already set";
			} else if (num == kERR_UNKNOWNMODE) {
				msg += "472 " + user.getNickName() + " " + parsedMsg.getParams()[1].getValue() + " :is unknown mode char to me for " + parsedMsg.getParams()[0].getValue();
			} else if (num == kERR_NOCHANMODES) {
				// TOPIC
				msg += "477 " + user.getNickName() + " " + parsedMsg.getParams()[0].getValue() + " :Channel doesn't support modes";
			} else if (num == kERR_CHANOPRIVSNEEDED) {
				msg += "482 " + user.getNickName() + " " + parsedMsg.getParams()[0].getValue() + " :You're not channel operator";
			} else if (num == kERR_RESTRICTED) {
				msg += "484 " + user.getNickName() + " :Your connection is restricted!";
			} else if (num == kERR_UMODEUNKNOWNFLAG) {
				msg += "501 " + user.getNickName() + " :Unknown MODE flag";
			} else if (num == kERR_USERSDONTMATCH) {
				msg += "502 " + user.getNickName() + " :Cannot change mode for other users";
			}
		}
		msg += this->delimiter_;
		return (msg);
	} catch (const std::exception& e) {
		printErrorMessage(e.what());
		return ("");
	}
}
