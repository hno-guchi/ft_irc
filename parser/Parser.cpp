#include "./Parser.hpp"
#include "../color.hpp"
#include "../error/error.hpp"

/*
 * 拡張BNF記法
 * message	= [ ":" prefix SPACE ] command [ params ] crlf
 *
 * prefix	=	servername / ( nickname [ [ "!" user ] "@" host ] )
 * SPACE	=	%x20				; space character
 * command	=	1*letter / 3digit
 * params	=	*14( SPACE middle ) [ SPACE ":" trailing ]
 * 			=/	14( SPACE middle ) [ SPACE [ ":" ] trailing ]
 * crlf		=	%x0D %x0A			; "carriage return" "linefeed"
 *
 *
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
 * PONG <server> [ <server2> ]
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

static bool	isNotAlpha(const char& c) {
	std::locale	l = std::locale::classic();
	return (!std::isalpha(c, l));
}

static bool	isNotDigit(const char& c) {
	std::locale	l = std::locale::classic();
	return (!std::isdigit(c, l));
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

void	ParsedMessage::setCommand(const std::string &command) {
	if (command.empty()) {
		throw std::invalid_argument("Empty command.");
	}
	try {
		if (isFuncString(command, isNotAlpha)) {
			// TODO(hnoguchi): Check exist command ?? (Now Execute command)
			this->type_ = kString;
			this->command_ = toUpperString(command);
		} else if (isFuncString(command, isNotDigit)) {
			// TODO(hnoguchi): Check exist digit
			this->type_ = kDigit;
			this->command_ = command;
		} else {
			throw std::invalid_argument("Command is not alphabet or digit.");
		}
	} catch (std::exception& e) {
		throw;
	}
}

/*
 * middle     =  nospcrlfcl *( ":" / nospcrlfcl )
 * trailing   =  *( ":" / " " / nospcrlfcl )
 * １文字目が、コロンかスペースならばtrailing
 * １文字目がnospcrlfclで、残りの全ての文字列がコロンかnospcrlfclならばmiddle。
 * １文字目がnospcrlfclで、残りの文字列にスペースがあれば、trailing。
 */
void	ParsedMessage::setParam(const Token& token) {
	// kTrailing typeでは、空文字列がくる場合がある。
	// if (token.getValue().empty()) {
	// 	printErrorMessage("Empty param.");
	// 	return;
	// }
	try {
		Param	p;
		// TODO(hnoguchi): Commandで条件分岐、各コマンドに対応したチェックを行う。
		if (token.getType() == kMiddle) {
			p.setType(kPMiddle);
		} else {
			p.setType(kPTrailing);
		}
		p.setValue(token.getValue());
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
		std::string			word;
		std::istringstream	msgStream(*message);
		while (std::getline(msgStream, word, ' ')) {
			if (word.empty()) {
				continue;
			}
			Token	token;
			if (!tokens->empty()) {
				if (word[0] == ':') {
					// Trailing
					word = word.substr(1);
					std::string	remaining;

					std::getline(msgStream, remaining);
					// TODO(hnoguchi): Check std::getline() error
					if (remaining.size() > 0) {
						word += " ";
						word += remaining;
					}
					// TODO(hnoguchi): Trailingは、空文字列を許容するかBNF確認すること。
					token.setType(kTrailing);
				} else {
					// Middle
					token.setType(kMiddle);
				}
			} else {
				token.setType(kCommand);
			}
			token.setValue(word);
			tokens->push_back(token);
		}
		// TODO(hnoguchi): Check std::getline() error
	} catch (std::exception& e) {
		throw;
	}
}

int	Parser::parse(std::string message) {
	if (message.empty()) {
		// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
		return (-1);
	}
	try {
		// TODO(hnoguchi): TokenList classを作成した方が良い？
		// TokenList	tokenList(&message);
		std::vector<Token>	tokens;

		tokenize(&message, &tokens);
		if (tokens.empty()) {
			// TODO(hnoguchi): 適切なエラーリプライナンバーを返す。
			return (-1);
		}
		for (std::vector<Token>::const_iterator it = tokens.begin(); it != tokens.end(); it++) {
			if (it->getType() == kMiddle || it->getType() == kTrailing) {
				this->parsed_.setParam(*it);
			} else if (it->getType() == kCommand) {
				this->parsed_.setCommand(it->getValue());
			}
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

#ifdef DEBUG

std::vector<std::string>	split(const std::string& message, \
		const std::string delim) {
	std::vector<std::string>	messages;
	std::string::size_type		startPos(0);

	while (startPos < message.size()) {
		std::string::size_type	delimPos = message.find(delim, startPos);
		if (delimPos == message.npos) {
			break;
		}
		std::string	buf = message.substr(startPos, delimPos - startPos);
		messages.push_back(buf);
		startPos = delimPos + delim.size();
	}
	return (messages);
}

void	printMessages(const std::vector<std::string>& messages) {
	if (messages.empty()) {
		return;
	}
	std::cout << "[Splited]   ____________________" << std::endl;
	for (std::vector<std::string>::const_iterator it = messages.begin(); \
			it != messages.end(); it++) {
		std::cout << "[" << *it << "]" << std::endl;
	}
	std::cout << std::endl;
	return;
}

int	main() {
	std::string	testMessageList[] = {
		"COMMAND param1 :param2\r\nCOMMAND param1 :param2\r\nCOMMAND param1 :param2\r\n",
		"COMMAND param1 param2 param3\r\n",
		"   COMMAND   param1    param2     param3    \r\n",
		"",
		"     ",
		"COMMAND\r\n",
		"COMMAND \r\n",
		"command\r\n",
		"coMMAnd\r\n",
		"111\r\n",
		"11a \r\n",
		"11\r\n",
		"COMMAND param1 :param2\r\n",
		"COMMAND param1 :param2\r\nCOMMAND param1 :param2\r\nCOMMAND param1 :param2\r\n   COMMAND param1 :param2\r\nCOMMAND param1 :param2\r\n",
		"COMMAND param1 :param2 param3 param4 param5\r\nCOMMAND param1 :param2    param3\r\nCOMMAND param1 :param2 :param3 :param4\r\n   COMMAND param1 :param2     :param3     param4     :param5\r\nCOMMAND :param1 param2\r\n"
	};

	for (size_t i = 0; \
			i < sizeof(testMessageList) / sizeof(testMessageList[0]); i++) {
		std::cout << "[Message]    ____________________" << std::endl;
		std::cout << GREEN << testMessageList[i] << END << std::endl;
		std::cout << "_________________________________" << std::endl;

		// TODO(hnoguchi): crlfをデリミタに、メッセージの分割処理を実装する（複数のメッセージが一度に来る場合がある。）
		// TODO(hnoguchi): 終端にcrlfがあるかチェックする。
		// TODO(hnoguchi): メッセージの長さが512文字(crlfを含む)を超えていないかチェックする。

		std::vector<std::string>	messages = split(testMessageList[i], "\r\n");
		printMessages(messages);

		for (std::vector<std::string>::const_iterator it = messages.begin(); it != messages.end(); it++) {
			Parser	parser;

			// parser.tokenize();
			// parser.printTokens();
			parser.parse(*it);
			parser.getParsedMessage().printParsedMessage();
			std::cout << YELLOW << "[Execute]   ____________________" << END << std::endl;
			std::cout << YELLOW << "[Reply]     ____________________" << END << std::endl;
			std::cout << std::endl;
		}
	}
#ifdef LEAKS
	system("leaks -q parser");
#endif  // LEAKS
	return(0);
}
#endif  // DEBUG
