#include <iostream>
#include <iomanip>
#include <sstream>
#include "./ValidParam.hpp"
#include "./IsChar.hpp"
#include "../color.hpp"

// middle		=	nospcrlfcl *( ":" / nospcrlfcl )
// nospcrlfcl	=	%x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
// ; any octet except NUL, CR, LF, " " and ":"
bool	ValidParam::isMiddle(const std::string &middle) {
	if (middle.empty()) {
		return (false);
	}
	if (middle[0] == ':') {
		return (false);
	}
	for (size_t i = 1; i < middle.size(); i++) {
		if (this->isNullCrLfSpace(middle[i])) {
			return (false);
		}
	}
	return (true);
}

// trailing		=	*( ":" / " " / nospcrlfcl )
// nospcrlfcl	=	%x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
// ; any octet except NUL, CR, LF, " " and ":"
bool	ValidParam::isTrailing(const std::string &trailing) {
	if (trailing.empty()) {
		return (false);
		// return (true);
	}
	if (trailing[0] != ':') {
		return (false);
	}
	for (size_t i = 1; i < trailing.size(); i++) {
		if (this->isNullCrLfSpace(trailing[i]) && trailing[i] != ' ') {
			return (false);
		}
	}
	return (true);
}

bool	ValidParam::isCommand(const std::string& command, const std::string* cmdList) {
	for (size_t i = 0; cmdList[i] != ""; i++) {
		if (command == cmdList[i]) {
			return (true);
		}
	}
	return (false);
}

bool	ValidParam::isPassword(const std::string& password) {
	if (password.size() < 6 || password.size() > 20) {
		return (false);
	}
	for (size_t i = 0; i < password.size(); i++) {
		if (!std::isalpha(password[i]) && !std::isdigit(password[i])) {
			return (false);
		}
	}
	return (true);
}

// nickname	=	( letter / special ) *8( letter / digit / special / "-" )
// letter	=	%x41-5A / %x61-7A	; A-Z / a-z
// digit	=	%x30-39				; 0-9
// special	=	%x5B-60 / %x7B-7D	; "[", "]", "\", "`", "_", "^", "{", "|", "}"
bool	ValidParam::isNickName(const std::string& nickName) {
	if (nickName.size() < 1 || nickName.size() > 9) {
		return (false);
	}
	std::locale	l = std::locale::classic();
	if (!std::isalpha(nickName[0], l) && !this->isSpecial(nickName[0])) {
		return (false);
	}
	for (size_t i = 1; i < nickName.size(); i++) {
		if (std::isalpha(nickName[i], l)) {
			continue;
		}
		if (std::isdigit(nickName[i], l)) {
			continue;
		}
		if (this->isSpecial(nickName[i]) || nickName[i] == '-') {
			continue;
		}
		return (false);
	}
	return (true);
}

// user = 1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )
// ; any octet except NUL, CR, LF, " " and "@"
bool	ValidParam::isUser(const std::string& user) {
	if (user.size() < 1  || user.size() > 20) {
		return (false);
	}
	for (size_t i = 0; i < user.size(); i++) {
		if (this->isNullCrLfSpace(user[i]) || user[i] == '@') {
			return (false);
		}
	}
	return (true);
}

// shortname = ( letter / digit ) *( letter / digit / "-" ) *( letter / digit )
// ; as specified in RFC 1123 [HNAME]
bool	ValidParam::isShortName(const std::string& shortName) {
	std::locale	l = std::locale::classic();

	if (!std::isalpha(shortName[0], l) && !std::isdigit(shortName[0], l)) {
		return (false);
	}
	for (size_t i = 1; i < (shortName.size() - 1); i++) {
		if (std::isalpha(shortName[0], l) || std::isdigit(shortName[0], l)) {
			continue;
		}
		if (shortName[i] == '-') {
			continue;
		}
		return (false);
	}
	if (!std::isalpha(shortName[shortName.size() - 1], l) && !std::isdigit(shortName[shortName.size() - 1], l)) {
		return (false);
	}
	return (true);
}

// hostname = shortname *( "." shortname )
// 最大６３文字。これを超える場合は、<hostaddr>を使用する。
bool	ValidParam::isHostName(const std::string& hostName) {
	if (hostName.size() > 63) {
		return (false);
	}
	std::string			shortName("");
	std::istringstream	nameStream(hostName);
	std::cin.clear(std::ios::goodbit);
	while (std::getline(nameStream, shortName, '.')) {
		if (std::cin.fail()) {
			return (false);
		}
		if (shortName.empty()) {
			return (false);
		}
		if (!this->isShortName(shortName)) {
			return (false);
		}
	}
	if (std::cin.fail()) {
		return (false);
	}
	return (true);
}

// channel		=	"#" *50( chanstring )
// chanstring	=	%x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B / %x2D-39 / %x3B-FF
// ; any octet except NUL, BELL, CR, LF, " ", "," and ":"
// チャンネル名は大文字、小文字を区別しない
bool	ValidParam::isChannel(const std::string& channel) {
	if (channel.size() > 50) {
		return (false);
	}
	if (channel[0] != '#') {
		return (false);
	}
	for (size_t i = 1; i < channel.size(); i++) {
		if (!this->isChanStringChar(channel[i])) {
			return (false);
		}
	}
	return (true);
}

// chanstring	=	%x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B / %x2D-39 / %x3B-FF
// ; any octet except NUL, BELL, CR, LF, " ", "," and ":"
bool	ValidParam::isMessage(const std::string& message) {
	if (message.size() > 510) {
		return (false);
	}
	for (size_t i = 0; i < message.size(); i++) {
		if (!this->isChanStringChar(message[i])) {
			return (false);
		}
	}
	return (true);
}

// chanstring	=	%x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B / %x2D-39 / %x3B-FF
// ; any octet except NUL, BELL, CR, LF, " ", "," and ":"
bool	ValidParam::isTopic(const std::string& topic) {
	if (topic.size() > 30) {
		return (false);
	}
	for (size_t i = 0; i < topic.size(); i++) {
		if (!this->isChanStringChar(topic[i])) {
			return (false);
		}
	}
	return (true);
}

// key = 1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
// ; any 7-bit US_ASCII character, except NUL, CR, LF, FF, h/v TABs, and " "
bool	ValidParam::isKey(const std::string& key) {
	if (key.size() < 1 || key.size() > 23) {
		return (false);
	}
	for (size_t i = 0; i < key.size(); i++) {
		if (key[i] == 6 || key[i] == 9 || key[i] == 11) {
			return (false);
		}
		if (this->isNullCrLfSpace(key[i])) {
			return (false);
		}
	}
	return (true);
}

// #ifdef UTEST_VALID
// void	test(bool (*func)(const char)) {
// 	std::cout.unsetf(std::ios::showbase);
// 	std::cout.setf(std::ios::uppercase);
// 	std::cout.setf(std::ios::hex, std::ios::basefield);
// 	for (char c = 0; c < 16; c++) {
// 		if (func(c)) {
// 			std::cout << GREEN << "0X0" << (int)c << END << ", " << std::flush;
// 		} else {
// 			std::cout << RED << "0X0" << (int)c << END << ", " << std::flush;
// 		}
// 	}
// 	std::cout.setf(std::ios::showbase);
// 	for (char c = 16; c < 127; c++) {
// 		if (c % 16 == 0) {
// 			std::cout << std::endl;
// 		}
// 		if (func(c)) {
// 			std::cout << GREEN << (int)c << END << ", " << std::flush;
// 		} else {
// 			std::cout << RED << (int)c << END << ", " << std::flush;
// 		}
// 	}
// 	if (func(127)) {
// 		std::cout << GREEN << 127 << END << std::flush;
// 	} else {
// 		std::cout << RED << 127 << END << std::flush;
// 	}
// 	std::cout << std::endl;
// }
// 
// int main() {
// 	std::cout << "[isNotAlpha] ____________________" << std::endl;
// 	test(ValidParam::isNotAlpha);
// 	std::cout << "_________________________________" << std::endl;
// 
// 	std::cout << "[isNotDigit] ____________________" << std::endl;
// 	test(ValidParam::isNotDigit);
// 	std::cout << "_________________________________" << std::endl;
// 
// 	std::cout << "[isSpecialChar] _________________" << std::endl;
// 	std::cout << " 0X5B-0X60 / 0X7B-0X7D ; [, ], \\, `, _, ^, {, |, }" << std::endl;
// 	test(ValidParam::isSpecialChar);
// 	std::cout << "_________________________________" << std::endl;
// 
// 	std::cout << "[isNullCrLfSpaceChar] ___________" << std::endl;
// 	std::cout << "any octet except NUL, CR, LF, \" \"" << std::endl;
// 	test(isNullCrLfSpaceChar);
// 	std::cout << "_________________________________" << std::endl;
// 	
// 	std::cout << "[isNoSpCrLfClChar] _______" << std::endl;
// 	std::cout << "0X01-0X09 / 0X0B-0X0C / 0X0E-0X1F / 0X21-0X39 / 0X3B-0XFF" << std::endl;
// 	std::cout << "any octet except NUL, CR, LF, \" \" and : " << std::endl;
// 	test(ValidParam::isNoSpCrLfClChar);
// 	std::cout << "_________________________________" << std::endl;
// 	std::cout << "[isChanStringChar] ______________" << std::endl;
// 	std::cout << " 0X01-0X07 / 0X08-0X09 / 0X0B-0X0C / 0X0E-0X1F / 0X21-0X2B / 0X2D-0X39 / 0X3B-0XFF " << std::endl;
// 	std::cout << "any octet except NUL, BELL, CR, LF, \" \", \",\" and :" << std::endl;
// 	test(ValidParam::isChanStringChar);
// 	std::cout << "_________________________________" << std::endl;
// 	std::cout << "any octet except NUL, CR, LF, \" \" and @" << std::endl;
// #ifdef DEBUG
// 	system("leaks validparam");
// #endif  // DEBUG
// }
// 
// #endif  // UTEST
