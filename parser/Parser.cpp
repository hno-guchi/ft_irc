#include "./Parser.hpp"
#include "../color.hpp"
#include "../error/error.hpp"

/*
 * 拡張BNF記法
 * message	= [ ":" prefix SPACE ] command [ params ] crlf
 *
 * command	= 1*letter / 3digit
 *
 * letter	= %x41-5A / %x61-7A	; A-Z / a-z
 * digit	= %x30-39			; 0-9
 *
 * params	= *14( SPACE middle ) [ SPACE ":" trailing ]
 * 			=/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]
 *
 * middle     =  nospcrlfcl *( ":" / nospcrlfcl )
 * trailing   =  *( ":" / " " / nospcrlfcl )
 *
 * nospcrlfcl = %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
 * 				; any octet except NUL, CR, LF, " " and ":"
 *
 */

// Token class
Token::Token() {}
Token::~Token() {}
void	Token::setType(const kTokenType type) {
	this->type_ = type;
	return;
}

// SETTER
void	Token::setValue(const std::string &value) {
	this->value_ = value;
	return;
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
	return;
}

// Param class
Param::Param() {}
Param::~Param() {}

void	Param::setType(const kParamType type) {
	this->type_ = type;
	return;
}

// SETTER
void	Param::setValue(const std::string &value) {
	this->value_ = value;
	return;
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
	return;
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
	return (std::find_if(str.begin(), str.end(), func) == str.end());
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
		printErrorMessage("Command is empty.");
		return;
	}
	if (isFuncString(command, isNotAlpha)) {
		// TODO(hnoguchi): Check exist command ?? (Now Execute command)
		this->type_ = kString;
		this->command_ = toUpperString(command);
	} else if (isFuncString(command, isNotDigit)) {
		// TODO(hnoguchi): Check exist digit
		this->type_ = kDigit;
		this->command_ = command;
	} else {
		printErrorMessage("Command is not alphabet or digit.");
	}
	return;
}

/*
 * middle     =  nospcrlfcl *( ":" / nospcrlfcl )
 * trailing   =  *( ":" / " " / nospcrlfcl )
 * １文字目が、コロンかスペースならばtrailing
 * １文字目がnospcrlfclで、残りの全ての文字列がコロンかnospcrlfclならばmiddle。
 * １文字目がnospcrlfclで、残りの文字列にスペースがあれば、trailing。
 */
void	ParsedMessage::setParam(const Token& token) {
	if (token.getValue().empty()) {
		printErrorMessage("Empty param.");
		return;
	}
	Param	p;
	// TODO(hnoguchi): Commandで条件分岐、各コマンドに対応したチェックを行う。
	if (token.getType() == kMiddle) {
		p.setType(kPMiddle);
	} else {
		p.setType(kPTrailing);
	}
	p.setValue(token.getValue());
	this->params_.push_back(p);
	return;
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
	if (message->empty()) {
		printErrorMessage("Message is empty.");
		return;
	}
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
				std::string	remaining;

				std::getline(msgStream, remaining);
				word = word.substr(1);
				word += " ";
				word += remaining;
				token.setType(kTrailing);
			} else {
				// middle
				token.setType(kMiddle);
			}
		} else {
			token.setType(kCommand);
		}
		token.setValue(word);
		tokens->push_back(token);
	}
	// TODO(hnoguchi): Check std::getline() error
	return;
}

void	Parser::parse(std::string message) {
	std::vector<Token>	tokens;

	tokenize(&message, &tokens);
	if (tokens.empty()) {
		printErrorMessage("Empty tokens;");
		return;
	}
	for (std::vector<Token>::const_iterator it = tokens.begin(); it != tokens.end(); it++) {
		if (it->getType() == kMiddle || it->getType() == kTrailing) {
			this->parsed_.setParam(*it);
		} else if (it->getType() == kCommand) {
			this->parsed_.setCommand(it->getValue());
		}
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
