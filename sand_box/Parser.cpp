#include "./Parser.hpp"
#include "../color.hpp"
#include "../error/error.hpp"

/*
 * 拡張BNF記法
 * message	= [ ":" prefix SPACE ] command [ params ] crlf
 *
 * prefix	=	servername / ( nickname [ [ "!" user ] "@" host ] )
 * SPACE	=	%x20				; space character
 * command	=	1*letter // command	=	1*letter / 3digit
 * params	=	*14( SPACE middle ) [ SPACE ":" trailing ]
 * 			=/	14( SPACE middle ) [ SPACE [ ":" ] trailing ]
 * crlf		=	%x0D %x0A			; "carriage return" "linefeed"
 *
 * channel		=	"#" *50( chanstring )	// channel = ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring [ ":" chanstring ]
 * servername	=	hostname
 * nickname		=	( letter / special ) *8( letter / digit / special / "-" )
 * user			=	1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )	; any octet except NUL, CR, LF, " " and "@"
 * host			=	hostname			// host = hostname / hostaddr
 *
 *
 * hostname		=	shortname *( "." shortname ) // 最大６３文字。これを超える場合は、<hostaddr>を使用する。
 * shortname	=	( letter / digit ) *( letter / digit / "-" ) *( letter / digit )	; as specified in RFC 1123 [HNAME]
 *
 *
 * letter	=	%x41-5A / %x61-7A		; A-Z / a-z
 * digit	=	%x30-39				; 0-9
 * special	=	%x5B-60 / %x7B-7D	; "[", "]", "\", "`", "_", "^", "{", "|", "}"
 *
 * middle	=	nospcrlfcl *( ":" / nospcrlfcl )
 * trailing	=	*( ":" / " " / nospcrlfcl )
 *
 * nospcrlfcl	=	%x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
 * 					; any octet except NUL, CR, LF, " " and ":"
 *
 * chanstring	=	%x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B
 * chanstring	=/	%x2D-39 / %x3B-FF	; any octet except NUL, BELL, CR, LF, " ", "," and ":"
 * key			=	1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
 * 					; any 7-bit US_ASCII character,
 * 					; except NUL, CR, LF, FF, h/v TABs, and " "
 *
 *
 *
 * hostname
 * 最大６３文字。これを超える場合は、<hostaddr>を使用する。
 *
 * <channel>
 * prefixは"#"のみ
 * 最大５０文字
 * チャンネル名に、スペース(「　」)、コントロールG(^GまたはASCII code 7)、カンマ(「 , 」)、ダブルころん（":"）を含んではならない
 * チャンネル名は大文字、小文字を区別しない
 *
 * CONNECTION_REGISTRATION
 * PASS <password>
 * NICK <nickname>
 * USER <username> <hostname> <servername> <realname>
 * OPER <name> <password>
 * QUIT [ <Quit Message> ]
 * MODE <nickname> ( "+" / "-" ) ( "o" )				// MODE <nickname> *( ( "+" / "-" ) *( "i" / "w" / "o" / "O" / "r" ) )
 *
 * CHANNEL_OPERATIONS
 * MODE <channel> ( "-" / "+" ) <mode> [ <modeparam> ]	// MODE <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )
 * INVITE <nickname> <channel>
 * JOIN <channel> [ <key> ]								// JOIN ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
 * KICK <channel> <user> [<comment>]					// KICK <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]
 * PART <channel> [ <Part Message> ]					// PART <channel> *( "," <channel> ) [ <Part Message> ]
 * TOPIC <channel> [ <topic> ]
 *
 * MISCELLANEOUS
 * PING <server1> [ <server2> ]
 * PONG <server1> [ <server2> ]
 *
 * SENDING
 * NOTICE <msgtarget> <text>
 * PRIVMSG <msgtarget> <text>
 */

// Token class
Token::Token() {}
Token::~Token() {}

void	Token::setType(const kTokenType type) {
	this->type_ = type;
}

// SETTER
void	Token::setValue(const std::string &value) {
	this->value_ = value;
}

// GETTER
kTokenType	Token::getType() const {
	return (this->type_);
}

const std::string&	Token::getValue() const {
	return (this->value_);
}

// debug
void	Token::printToken() const {
	std::cout << "Type   : " << this->type_ << " | " << std::flush;
	std::cout << "Value  : [" << this->value_ << "]" << std::endl;
}

// Param class
Param::Param() {}
Param::~Param() {}

void	Param::setType(const kParamType type) {
	this->type_ = type;
}

// SETTER
void	Param::setValue(const std::string &value) {
	this->value_ = value;
}

// GETTER
kParamType	Param::getType() const {
	return (this->type_);
}

const std::string&	Param::getValue() const {
	return (this->value_);
}

// debug
void	Param::printParam() const {
	std::cout << "Type   : " << this->type_ << " | " << std::flush;
	std::cout << "Value  : [" << this->value_ << "]" << std::endl;
}

// ParsedMessage class
ParsedMessage::ParsedMessage() {}
ParsedMessage::~ParsedMessage() {}

static bool	isNotAlpha(const char c) {
	std::locale	l = std::locale::classic();
	return (!std::isalpha(c, l));
}

static bool	isNotDigit(const char c) {
	std::locale	l = std::locale::classic();
	return (!std::isdigit(c, l));
}

// special	=	%x5B-60 / %x7B-7D	; "[", "]", "\", "`", "_", "^", "{", "|", "}"
static bool	isSpecialChar(const char c) {
	if (c >= 91 && c <= 96) {
		return (true);
	}
	if (c >= 123 && c <= 125) {
		return (true);
	}
	return (false);
}

// nospcrlfcl	=	%x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF ; any octet except NUL, CR, LF, " " and ":"
static bool	isNotSpaceCrLfColonChar(const char c) {
	if (c == 0 || c == 10 || c == 13 || c == 32 || c == 58) {
		return (false);
	}
	return (true);
}

static bool	isFuncString(const std::string &str, bool (*func)(const char&)) {
	try {
		return (std::find_if(str.begin(), str.end(), func) == str.end());
	} catch (std::exception& e) {
		throw;
	}
}

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

void	ParsedMessage::setCommand(const std::string &command) {
	if (command.empty()) {
		throw std::invalid_argument("Empty command.");
	}
	try {
		if (isFuncString(command, isNotAlpha)) {
			// TODO(hnoguchi): Check exist command ?? (Now Execute command)
			this->type_ = kCmdString;
			this->command_ = toUpperString(command);
		} else if (isFuncString(command, isNotDigit)) {
			// TODO(hnoguchi): Check exist digit
			this->type_ = kCmdDigit;
			this->command_ = command;
		} else {
			throw std::invalid_argument("Command is not alphabet or digit.");
		}
	} catch (std::exception& e) {
		throw;
	}
}

// middle     =  nospcrlfcl *( ":" / nospcrlfcl )
// trailing   =  *( ":" / " " / nospcrlfcl )
void	ParsedMessage::setParam(kParamType type, const std::string& value) {
// void	ParsedMessage::setParam(const Token& token) {
	// kTrailing typeでは、空文字列がくる場合がある。
	// if (token.getValue().empty()) {
	// 	printErrorMessage("Empty param.");
	// 	return;
	// }
	try {
		Param	p;
		p.setType(type);
		p.setValue(value);
		this->params_.push_back(p);
	} catch (std::exception& e) {
		throw;
	}
}

const std::string&	ParsedMessage::getCommand() const {
	return (this->command_);
}

const std::vector<Param>&	ParsedMessage::getParams() const {
	return (this->params_);
}

void	ParsedMessage::printParsedMessage() const {
	if (this->command_.empty()) {
		return;
	}
	std::cout << "[Parsed]    ____________________" << std::endl;
	std::cout << "Type   : " << this->type_ << " | " << std::flush;
	std::cout << "Command: [" << this->command_ << "]" << std::endl;
	if (this->params_.empty()) {
		return;
	}
	for (std::vector<Param>::const_iterator it = this->params_.begin(); \
			it != this->params_.end(); it++) {
		it->printParam();
	}
	return;
}

// Parser class
Parser::Parser() {}
Parser::~Parser() {}

static void	tokenize(std::string *message, std::vector<Token> *tokens) {
	try {
		std::string			word("");
		std::istringstream	msgStream(*message);
		while (word.empty()) {
			std::getline(msgStream, word, ' ');
		}
		// validation command
		Token	command;
		command.setType(kCmdString);
		command.setValue(word);
		tokens->push_back(token);
		while (std::getline(msgStream, word, ' ')) {
			if (word.empty()) {
				continue;
			}
			Token	param;
			if (word[0] != ':') {
				// Middle
				// validation paramMiddle
				param.setType(kParamMiddle);
			} else {
				word = word.substr(1);
				std::string	trailing("");

				std::getline(msgStream, trailing);
				// TODO(hnoguchi): Check std::getline() error
				if (trailing.size() > 0) {
					word += " " + trailing;
				}
				// TODO(hnoguchi): Trailingは、空文字列を許容するかBNF確認すること。
				// validation paramTrailing
				param.setType(kParamTrailing);
			}
			param.setValue(word);
			tokens->push_back(param);
		}
		// TODO(hnoguchi): Check std::getline() error
	} catch (std::exception& e) {
		throw;
	}
}

bool	Parser::isCommand(const std::string& command, const std::string* cmdList) {
	for (size_t i = 0; cmdList[i] != ""; i++) {
		if (command == cmdList[i]) {
			return (true);
		}
	}
	return (false);
}

// nickname	=	( letter / special ) *8( letter / digit / special / "-" )
// letter	=	%x41-5A / %x61-7A	; A-Z / a-z
// digit	=	%x30-39				; 0-9
// special	=	%x5B-60 / %x7B-7D	; "[", "]", "\", "`", "_", "^", "{", "|", "}"
static bool	isParamNickName(const std::string& nickName) {
	if (nickName.size() > 9) {
		return (false);
	}
	std::locale	l = std::locale::classic();
	if (!std::isalpha(nickName[0], l) && !isSpecialChar(nickName[0])) {
		return (false);
	}
	for (size_t i = 1; i < nickName.size(); i++) {
		if (std::isalpha(nickName[i], l)) {
			continue;
		}
		if (std::isdigit(nickName[i], l)) {
			continue;
		}
		if (isSpecialChar(nickName[i])) {
			continue;
		}
		return (false);
	}
	return (true);
}

// user = 1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )	; any octet except NUL, CR, LF, " " and "@"
static bool	isParamUser(const std::string& user) {
	if (user.size() < 1  || user.size() > 20) {
		return (false);
	}
	for (size_t i = 0; i < user.size(); i++) {
		if (user[i] >= 1 && user[i] <= 9) {
			continue;
		}
		if (user[i] == 11 || user[i] == 12) {
			continue;
		}
		if (user[i] >= 14 && user[i] <= 31) {
			continue;
		}
		if (user[i] >= 33 && user[i] <= 63) {
			continue;
		}
		if (user[i] >= 65 && user[i] <= 255) {
			continue;
		}
		return (false);
	}
	return (true);
}

// hostname		=	shortname *( "." shortname ) // 最大６３文字。これを超える場合は、<hostaddr>を使用する。
// shortname	=	( letter / digit ) *( letter / digit / "-" ) *( letter / digit )	; as specified in RFC 1123 [HNAME]
static bool	isShortName(const std::string& shortName) {
	std::locale	l = std::locale::classic();

	if (!std::isalpha(shortName[0], l) && !std::isDigit(shortName[0], l)) {
		return (false);
	}
	for (size_t i = 1; i < (shortName.size() - 1); i++) {
		if (std::isalpha(shortName[i], l)) {
			continue;
		}
		if (std::isdigit(shortName[i], l)) {
			continue;
		}
		if (shortName[i] == '-') {
			continue;
		}
		return (false);
	}
	if (!std::isalpha(shortName[shortName.size() - 1], l) && !std::isDigit(shortName[shortName.size() - 1], l)) {
		return (false);
	}
	return (true);
}

static bool	isParamHostName(const std::string& hostName) {
	if (hostName.size() > 63) {
		return (false);
	}
	std::string			shortName("");
	std::istringstream	nameStream(*hostName);
	while (std::getline(nameStream, shortName, '.')) {
		if (shortName.empty()) {
			return (false);
		}
		if (!isShortName(shortName)) {
			return (false);
		}
	}
	return (true);
}

static bool	isChanStringChar(const char c) {
	if (message[i] >= 1 && message[i] <= 7) {
		return (true);
	}
	if (message[i] == 8 || message[i] == 9) {
		return (true);
	}
	if (message[i] == 11 || message[i] == 12) {
		return (true);
	}
	if (message[i] >= 14 && message[i] <= 31) {
		return (true);
	}
	if (message[i] >= 33 && message[i] <= 43) {
		return (true);
	}
	if (message[i] >= 45 && message[i] <= 57) {
		return (true);
	}
	if (message[i] >= 59 && message[i] <= 255) {
		return (true);
	}
	return (false);
}

// channel		=	"#" *50( chanstring )
// chanstring	=	%x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B
// chanstring	=/	%x2D-39 / %x3B-FF	; any octet except NUL, BELL, CR, LF, " ", "," and ":"
// チャンネル名は大文字、小文字を区別しない
static bool	isParamChannel(const std::string& channel) {
	if (channel.size() > 50) {
		return (false);
	}
	if (channel[0] != '#') {
		return (false);
	}
	for (size_t i = 1; i < channel.size(); i++) {
		if (!isChanStringChar(channel[i])) {
			return (false);
		}
	}
	return (true);
}

// chanstring	=	%x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B
// chanstring	=/	%x2D-39 / %x3B-FF	; any octet except NUL, BELL, CR, LF, " ", "," and ":"
static bool	isParamMessage(const std::string& message) {
	if (message.size() > 510) {
		return (false);
	}
	for (size_t i = 0; i < message.size(); i++) {
		if (!isChanStringChar(channel[i])) {
			return (false);
		}
	}
	return (true);
}

// chanstring	=	%x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B
// chanstring	=/	%x2D-39 / %x3B-FF	; any octet except NUL, BELL, CR, LF, " ", "," and ":"
static bool	isParamTopic(const std::string& topic) {
	if (message.size() > 30) {
		return (false);
	}
	for (size_t i = 0; i < message.size(); i++) {
		if (!isChanStringChar(channel[i])) {
			return (false);
		}
	}
	return (true);
}

// key = 1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )	; any 7-bit US_ASCII character, except NUL, CR, LF, FF, h/v TABs, and " "
static bool	isParamKey(const std::string& key) {
	if (key.size() < 1 || key.size() > 23) {
		return (false);
	}
	for (size_t i = 0; i < key.size(); i++) {
		if (key[i] >= 1 && key[i] <= 5) {
			continue;
		}
		if (key[i] == 7 || key[i] == 8) {
			continue;
		}
		if (key[i] == 12) {
			continue;
		}
		if (key[i] >= 14 && key[i] <= 31) {
			continue;
		}
		if (key[i] >= 33 && key[i] <= 127) {
			continue;
		}
		return (false);
	}
	return (true);
}

int	Parser::parse(std::string message, const std::string* cmdList) {
// int	Parser::parse(std::string message, const Info& info) {
	if (message.empty()) {
		// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
		return (-1);
	}
	// check message length
	try {
		// TODO(hnoguchi): TokenList classを作成した方が良い？
		std::vector<Token>	tokens;

		tokenize(&message, &tokens);
		if (tokens.empty()) {
			// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
			return (-1);
		}
		// std::vector<Token>::const_iterator	it = tokens.begin();
		// if (it->getType() != kCmdString || !this->isCommand(this->parsed_.getCommand(), cmdList)) {
		if (tokens[0]->getType() != kCmdString || !this->isCommand(tokens[0]->getValue(), cmdList)) {
			// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
			return (-1);
		}
		this->parsed_.setCommand(tokens[0]->getValue());
		// command毎にパラメータのバリデーションを行う。
		if (this->parsed_.getCommand() == "PING") {
			// PING <server1> [ <server2> ]
			if (tokens.size() < 2 || tokens.size() > 3) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			this->parsed_.setParam(kServer, tokens[1]->getValue());
			if (tokens.size() == 3) {
				this->parsed_.setParam(kServer, tokens[2]->getValue());
			}
		} else if (this->parsed_.getCommand() == "PASS") {
			// PASS <password>
			if (tokens.size() != 2) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			this->parsed_.setParam(kPassword, tokens[1]->getValue());
		} else if (this->parsed_.getCommand() == "NICK") {
			// NICK <nickname>
			if (tokens.size() != 2) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			if (!isParamNickName(tokens[1]->getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
		} else if (this->parsed_.getCommand() == "USER") {
			// USER <username> <hostname> <servername> <realname>
			if (tokens.size() != 5) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			if (!isParamUser(tokens[1]->getValue())) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			this->parsed_.setParam(kUserName, tokens[1]->getValue());
			if (!isParamHostName(tokens[2]->getValue())) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			this->parsed_.setParam(kHostName, it->getValue());
			if (!isParamHostName(tokens[3]->getValue())) {
				// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
				return (-1);
			}
			this->parsed_.setParam(kHostName, tokens[3]->getValue());
			this->parsed_.setParam(kRealName, tokens[4]->getValue());
		} else if (this->parsed_.getCommand() == "OPER") {
			// OPER <name> <password>
			if (tokens.size() != 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!isParamNickName(tokens[1]->getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
			this->parsed_.setParam(kNickName, tokens[1]->getValue());
			this->parsed_.setParam(kPassword, tokens[2]->getValue());
		} else if (this->parsed_.getCommand() == "QUIT") {
			// QUIT [ <Quit Message> ]
			if (tokens.size() == 2) {
				if (isParamMessage(tokens[1]->getValue())) {
					this->parsed_.setParam(kMessage, tokens[1]->getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "JOIN") {
			// JOIN <channel> [ <key> ]
			if (tokens.size() < 2 || tokens.size() > 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!isParamChannel(tokens[1]->getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1]->getValue()));
			if (tokens.size() == 3) {
				if (!isParamKey(tokens[2]->getValue())) {
					// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
					return (-1);
				}
				this->parsed_.setParam(kKey, tokens[2]->getValue());
			}
		} else if (this->parsed_.getCommand() == "PART") {
			// PART <channel> [ <Part Message> ]
			if (tokens.size() < 2 || tokens.size() > 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!isParamChannel(tokens[1]->getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1]->getValue()));
			if (tokens.size() == 3) {
				if (isParamMessage(tokens[2]->getValue())) {
					this->parsed_.setParam(kMessage, tokens[2]->getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "KICK") {
			// KICK <channel> <user> [<comment>]
			if (tokens.size() != 3 && tokens.size() != 4) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!isParamChannel(tokens[1]->getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1]->getValue()));
			if (!isParamNickName(tokens[2]->getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
			this->parsed_.setParam(kNickName, tokens[2]->getValue());
			if (tokens.size() == 4) {
				if (isParamMessage(tokens[3]->getValue())) {
					this->parsed_.setParam(kMessage, tokens[3]->getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "INVITE") {
			// INVITE <nickname> <channel>
			if (tokens.size() != 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!isParamNickName(tokens[1]->getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
			this->parsed_.setParam(kNickName, tokens[1]->getValue());
			if (!isParamChannel(tokens[2]->getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[2]->getValue()));
		} else if (this->parsed_.getCommand() == "TOPIC") {
			// TOPIC <channel> [ <topic> ]
			if (tokens.size() != 2 && tokens.size() != 3) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!isParamChannel(tokens[1]->getValue())) {
				// 403 ERR_NOSUCHCHANNEL "<channel> :No such channel"
				return (-1);
			}
			this->parsed_.setParam(kChannel, toLowerString(tokens[1]->getValue()));
			if (tokens.size() == 3) {
				if (isParamTopic(tokens[2]->getValue())) {
					this->parsed_.setParam(kTopic, tokens[2]->getValue());
				}
			}
		} else if (this->parsed_.getCommand() == "MODE") {
			// MODE <nickname> [ ( "+" / "-" ) ( "o" ) ]
			// MODE <channel>  [ ( "-" / "+" ) ( "i" / "k" / "l" / "o" / "t" ) [ <modeparam> ] ]
			if (tokens.size() < 2 || tokens.size() > 4) {
				// 461 ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
				return (-1);
			}
			if (!isParamNickName(tokens[1]->getValue()) && !isParamChannel(tokens[1]->getValue())) {
				// 432 ERR_ERRONEUSNICKNAME "<nick> :Erroneous nickname"
				return (-1);
			}
			// tokens[1]->getValue()[0] != '#' ==> nickName

		} else if (this->parsed_.getCommand() == "PRIVMSG") {
			// PRIVMSG <msgtarget> <text>
		} else if (this->parsed_.getCommand() == "NOTICE") {
			// NOTICE <msgtarget> <text>
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

const ParsedMessage&	Parser::getParsedMessage() const {
	return (this->parsed_);
}
