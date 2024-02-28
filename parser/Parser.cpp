#include "./Parser.hpp"
#include "../color.hpp"

/*
 * 拡張BNF記法
 * message	= [ ":" prefix SPACE ] command [ params ] crlf
 *
 * command	= 1*letter / 3digit
 * 			letter	= %x41-5A / %x61-7A	; A-Z / a-z
 * 			digit	= %x30-39			; 0-9
 *
 */

static void	printErrorMessage(const std::string &message) {
	std::cerr << RED << message << END << std::endl;
	return;
}


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

// Command class
Command::Command() {}
Command::~Command() {}

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

void	Command::setCommand(const std::string &command) {
	if (command.empty()) {
		printErrorMessage("Command is empty.");
		return;
	}
	if (isFuncString(command, isNotAlpha)) {
		// TODO(hnoguchi): Check exist command
		this->type_ = kString;
		this->command_ = command;
	} else if (isFuncString(command, isNotDigit)) {
		// TODO(hnoguchi): Check exist digit
		this->type_ = kDigit;
		this->command_ = command;
	} else {
		printErrorMessage("Command is not alphabet and digit.");
	}
	return;
}

void	Command::setParam(const std::string &param) {
	if (param.empty()) {
		printErrorMessage("Empty param.");
		return;
	}
	this->params_.push_back(param);
	return;
}

const std::string&	Command::getCommand() const {
	return (this->command_);
}

const std::vector<std::string>&	Command::getParams() const {
	return (this->params_);
}

void	Command::printCommand() const {
	if (this->command_.empty()) {
		return;
	}
	std::cout << "Type   : " << this->type_ << " | " << std::flush;
	std::cout << "Command: [" << this->command_ << "]" << std::endl;
	if (this->params_.empty()) {
		return;
	}
	std::cout << "             " << std::flush;
	std::cout << "Param  : " << std::flush;
	for (std::vector<std::string>::const_iterator it = this->params_.begin(); \
			it != this->params_.end(); it++) {
		std::cout << "[" << *it << "], " << std::flush;
	}
	std::cout << std::endl;
	return;
}

// Parser class
Parser::Parser(const std::string &message) :
	message_(message) {
}

Parser::~Parser() {}

void	Parser::tokenize() {
	std::string	token;
	std::istringstream	tokenStream(this->message_);

	if (this->message_.empty()) {
		printErrorMessage("Message is empty.");
		return;
	}
	while (std::getline(tokenStream, token, ' ')) {
		if (token.empty()) {
			continue;
		}
		Token	t;
		if (!this->tokens_.empty()) {
			t.setType(kParam);
		} else {
			t.setType(kCommand);
		}
		t.setValue(token);
		this->tokens_.push_back(t);
	}
	// TODO(hnoguchi): Check std::getline() error
	return;
}

void	Parser::parse() {
	if (this->tokens_.empty()) {
		printErrorMessage("Empty this->tokens_;");
		return;
	}
	Command	command;
	for (std::vector<Token>::const_iterator it = this->tokens_.begin(); \
			it != this->tokens_.end(); it++) {
		if (it->getType() == kParam) {
			command.setParam(it->getValue());
		} else if (it->getType() == kCommand) {
			command.setCommand(it->getValue());
		}
	}
	this->commands_.push_back(command);
	command.printCommand();
}

// GETTER
const std::vector<Token>&	Parser::getTokens() const {
	return (this->tokens_);
}

void	Parser::printTokens() const {
	if (this->tokens_.empty()) {
		return;
	}
	std::cout << "[Tokenized] ____________________" << std::endl;
	for (std::vector<Token>::const_iterator it = tokens_.begin(); \
			it != tokens_.end(); it++) {
		it->printToken();
	}
	std::cout << std::endl;
	return;
}

void	Parser::printCommands() const {
	if (this->commands_.empty()) {
		return;
	}
	std::cout << "[Parsed]    ____________________" << std::endl;
	for (std::vector<Command>::const_iterator it = this->commands_.begin(); \
			it != this->commands_.end(); it++) {
		it->printCommand();
	}
	std::cout << std::endl;
	return;
}

int	main() {
	std::string	testMessageList[] = {
		"COMMAND param1 param2 param3",
		"   COMMAND    param1     param2   param3    ",
		"",
		"     ",
		"111 param1 param2 param3",
	};

	for (size_t i = 0; \
			i < sizeof(testMessageList) / sizeof(testMessageList[0]); i++) {
		std::cout << "Message: [" << testMessageList[i] << "]" << std::endl;

		Parser	parser(testMessageList[i]);
		parser.tokenize();
		parser.printTokens();
		parser.parse();
		parser.printCommands();

		// if (parser.getTokens().empty()) {
		// 	continue;
		// }
		// Command	command;
		// command.setCommand(parser.getTokens().front());
		// command.printCommand();
		// // command.setParams(parser.getTokens().front());
		// std::cout << std::endl;
	}
#ifdef LEAKS
	system("leaks -q parser");
#endif  // LEAKS
	return(0);
}
